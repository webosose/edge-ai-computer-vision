/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/faceMesh/FaceMeshDetector.h>
#include <aif/log/Logger.h>

namespace aif {

FaceMeshDetector::FaceMeshDetector(const std::string& modelPath)
    : Detector(modelPath)
{
}

FaceMeshDetector::~FaceMeshDetector()
{
}

std::shared_ptr<DetectorParam> FaceMeshDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<FaceMeshParam>();
    return param;
}

t_aif_status FaceMeshDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv image!!");
        }

        int height = m_modelInfo.height;
        int width = m_modelInfo.width;
        int channels = m_modelInfo.channels;

        if (m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter not initialized!!");
        }

        tflite::Interpreter* interpreter = m_interpreter.get();

        cv::Mat img_resized;
        if (img.cols < width || img.rows < height) // expand
        {
            cv::resize(img, img_resized, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
        }
        else // shrink...
        {
            cv::resize(img, img_resized, cv::Size(width, height), 0, 0, cv::INTER_AREA);
        }

        TRACE("resized size: ", img_resized.size());
        if (img_resized.rows != height || img_resized.cols != width) {
            Loge("image resize failed!!");
            return kAifError;
        }

        img_resized.convertTo(img_resized, CV_32FC3);

        // Normalization: 0 ~ 1
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                const auto &rgb = img_resized.at<cv::Vec3f>(i, j);
                if (interpreter->typed_input_tensor<float>(0) != nullptr) {
                    interpreter->typed_input_tensor<float>(0)[i * width * channels + j * channels + 0] = rgb[2] / 255;
                    interpreter->typed_input_tensor<float>(0)[i * width * channels + j * channels + 1] = rgb[1] / 255;
                    interpreter->typed_input_tensor<float>(0)[i * width * channels + j * channels + 2] = rgb[0] / 255;
                }
            }
        }

        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status FaceMeshDetector::preProcessing()
{
    return kAifOk;
}

void FaceMeshDetector::printOutput() {
    const std::vector<int>& outputs = m_interpreter->outputs();
    for (int i = 0; i < outputs.size(); i++) {
        TfLiteTensor *output= m_interpreter->tensor(outputs[i]);
        TRACE(i, ":output bytes: ", output->bytes);
        TRACE(i, ":output type: ", output->type);
        TRACE(i, ":output dims 0: ", output->dims->data[0]);
        TRACE(i, ":output dims 1: ",  output->dims->data[1]);
        TRACE(i, ":output dims 2: ",  output->dims->data[2]);
        TRACE(i, ":output dims 3: ",  output->dims->data[3]);
    }
}

t_aif_status FaceMeshDetector::postProcessing(const cv::Mat &img, std::shared_ptr<Descriptor> &descriptor)
{
    const std::vector<int> &tensor_outputs = m_interpreter->outputs();
    TfLiteTensor *tensor_output_coord = m_interpreter->tensor(tensor_outputs[0]);
    TfLiteTensor *tensor_output_score = m_interpreter->tensor(tensor_outputs[1]);

    const int CANONICAL_MESH_NUM_VERTICES_XYZ = tensor_output_coord->dims->data[3];

    std::shared_ptr<FaceMeshDescriptor> faceMeshDescriptor
        = std::dynamic_pointer_cast<FaceMeshDescriptor>(descriptor);

    std::vector<cv::Vec3f> landmarks;
    for (int i = 0; i < CANONICAL_MESH_NUM_VERTICES_XYZ; i += 3)
    {
        landmarks.push_back(
            cv::Vec3f(tensor_output_coord->data.f[i + 0] / m_modelInfo.width,
                      tensor_output_coord->data.f[i + 1] / m_modelInfo.height,
                      tensor_output_coord->data.f[i + 2] / m_modelInfo.width));
    }

    if (faceMeshDescriptor != nullptr)
    {
        float score = 1.0f / (1.0f + std::exp(-tensor_output_score->data.f[0]));
        faceMeshDescriptor->addLandmarks(landmarks, score);
    }

    return kAifOk;
}

} // end of namespace aif
