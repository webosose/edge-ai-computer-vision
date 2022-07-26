/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2013, OpenCV Foundation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#include <aif/face/YuNetFaceDetectorV2.h>
#include <aif/face/YuNetFaceParam.h>
#include <aif/log/Logger.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

using aif::Stopwatch;

namespace aif {

YuNetFaceDetectorV2::YuNetFaceDetectorV2(const std::string& modelName)
    : Detector(modelName) {}

YuNetFaceDetectorV2::~YuNetFaceDetectorV2() {}

std::shared_ptr<DetectorParam> YuNetFaceDetectorV2::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<YuNetFaceParam>();
    return param;
}

void YuNetFaceDetectorV2::setModelInfo(TfLiteTensor* inputTensor)
{
    m_modelInfo.batchSize = inputTensor->dims->data[0];
    m_modelInfo.height = inputTensor->dims->data[1];
    m_modelInfo.width = inputTensor->dims->data[2];
    m_modelInfo.channels = inputTensor->dims->data[3];

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);
}

t_aif_status YuNetFaceDetectorV2::preProcessing()
{
    try {
        generatePriors();
    } catch(const std::exception& e) {
        std::cerr << __func__ << "Error: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << __func__ << "Error: Unknown exception occured!!\n";
    }
     return kAifOk;
}

t_aif_status YuNetFaceDetectorV2::postProcessing(const cv::Mat &img,
        std::shared_ptr<Descriptor> &descriptor)
{

    try{
        Stopwatch sw;
        sw.start();
        if (faceDetect(descriptor) != kAifOk) {
            throw std::runtime_error("generateOutput failed!!");
        }
        TRACE("\tfaceDetect(): ", sw.getMs(), "ms");
        sw.stop();
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}


t_aif_status YuNetFaceDetectorV2::faceDetect(std::shared_ptr<Descriptor>& descriptor)
{
    try {
        std::shared_ptr<YuNetFaceParam> param = std::dynamic_pointer_cast<YuNetFaceParam>(m_param);
        if (m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter not initilaized!!");
        }

        const std::vector<int>& t_outputs = m_interpreter->outputs();
        TRACE("t_outputs.size: ", t_outputs.size());
        TfLiteTensor* loc = m_interpreter->tensor(t_outputs[0]);
        if (loc == nullptr) {
            throw std::runtime_error("can't get tflite loc!!");
        }
        TfLiteTensor* conf  = m_interpreter->tensor(t_outputs[1]);
        if (conf  == nullptr) {
            throw std::runtime_error("can't get tflite conf!!");
        }
        TfLiteTensor* iou = m_interpreter->tensor(t_outputs[2]);
        if (iou == nullptr) {
            throw std::runtime_error("can't get tflite iou!!");
        }

        const float scoreThreshold = param->scoreThreshold;
        const float nmsThreshold = param->nmsThreshold;
        const int topK = param->topK;

        // Decode from deltas and priors
        const std::vector<float> variance = {0.1f, 0.2f};
        float* loc_v = loc->data.f;
        float* conf_v = conf->data.f;
        float* iou_v = iou->data.f;

        cv::Mat faces;
        cv::Mat face(1, 15, CV_32FC1);
        for (size_t i = 0; i < m_priors.size(); ++i) {
            // Get score
            float clsScore = conf_v[i*2+1];
            float iouScore = iou_v[i];
            // Clamp
            if (iouScore < 0.f) {
                iouScore = 0.f;
            }
            else if (iouScore > 1.f) {
                iouScore = 1.f;
            }
            float score = std::sqrt(clsScore * iouScore);
            face.at<float>(0, 14) = score;

            // Get bounding box
            int paddedW = m_paddedSize.width;
            int paddedH = m_paddedSize.height;


            float cx = (m_priors[i].x + loc_v[i*14+0] * variance[0] * m_priors[i].width)  * paddedW;
            float cy = (m_priors[i].y + loc_v[i*14+1] * variance[0] * m_priors[i].height) * paddedH;
            float w  = m_priors[i].width  * exp(loc_v[i*14+2] * variance[0]) * paddedW;
            float h  = m_priors[i].height * exp(loc_v[i*14+3] * variance[1]) * paddedH;
            float x1 = cx - w / 2;
            float y1 = cy - h / 2;
            face.at<float>(0, 0) = x1;
            face.at<float>(0, 1) = y1;
            face.at<float>(0, 2) = w;
            face.at<float>(0, 3) = h;


            // Get landmarks
            face.at<float>(0, 4) = (m_priors[i].x + loc_v[i*14+ 4] * variance[0] * m_priors[i].width)  * paddedW;  // right eye, x
            face.at<float>(0, 5) = (m_priors[i].y + loc_v[i*14+ 5] * variance[0] * m_priors[i].height) * paddedH;  // right eye, y
            face.at<float>(0, 6) = (m_priors[i].x + loc_v[i*14+ 6] * variance[0] * m_priors[i].width)  * paddedW;  // left eye, x
            face.at<float>(0, 7) = (m_priors[i].y + loc_v[i*14+ 7] * variance[0] * m_priors[i].height) * paddedH;  // left eye, y
            face.at<float>(0, 8) = (m_priors[i].x + loc_v[i*14+ 8] * variance[0] * m_priors[i].width)  * paddedW;  // nose tip, x
            face.at<float>(0, 9) = (m_priors[i].y + loc_v[i*14+ 9] * variance[0] * m_priors[i].height) * paddedH;  // nose tip, y
            face.at<float>(0, 10) = (m_priors[i].x + loc_v[i*14+10] * variance[0] * m_priors[i].width)  * paddedW; // right corner of mouth, x
            face.at<float>(0, 11) = (m_priors[i].y + loc_v[i*14+11] * variance[0] * m_priors[i].height) * paddedH; // right corner of mouth, y
            face.at<float>(0, 12) = (m_priors[i].x + loc_v[i*14+12] * variance[0] * m_priors[i].width)  * paddedW; // left corner of mouth, x
            face.at<float>(0, 13) = (m_priors[i].y + loc_v[i*14+13] * variance[0] * m_priors[i].height) * paddedH; // left corner of mouth, y

            faces.push_back(face);
        }

        cv::Mat result;
        if (faces.rows > 1) {
            // Retrieve boxes and scores
            std::vector<cv::Rect2i> faceBoxes;
            std::vector<float> faceScores;
            for (int rIdx = 0; rIdx < faces.rows; rIdx++)
            {
                faceBoxes.push_back(cv::Rect2i(int(faces.at<float>(rIdx, 0)),
                            int(faces.at<float>(rIdx, 1)),
                            int(faces.at<float>(rIdx, 2)),
                            int(faces.at<float>(rIdx, 3))));
                faceScores.push_back(faces.at<float>(rIdx, 14));
            }

            std::vector<int> keepIdx;
            cv::dnn::NMSBoxes(faceBoxes, faceScores, scoreThreshold, nmsThreshold, keepIdx, 1.f, topK);

            // Get NMS results
            cv::Mat nms_faces;
            for (int idx: keepIdx) {
                nms_faces.push_back(faces.row(idx));
            }
            nms_faces.convertTo(result, CV_32FC1);
        } else {
            faces.convertTo(result, CV_32FC1);
        }
        convertToDescriptor(result, descriptor);
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }

    return kAifOk;
}

void YuNetFaceDetectorV2::convertToDescriptor(cv::Mat& faces, std::shared_ptr<Descriptor>& descriptor)
{
    int scaleX = m_scaleSize.width;
    int scaleY = m_scaleSize.height;
    std::shared_ptr<FaceDescriptor> faceDescriptor = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    for (int i = 0; i < faces.rows; i++) {
           faceDescriptor->addFace(
                    faces.at<float>(i, SCORE),
                    faces.at<float>(i, FACE_X)/scaleX,
                    faces.at<float>(i, FACE_Y)/scaleY,
                    faces.at<float>(i, FACE_W)/scaleX,
                    faces.at<float>(i, FACE_H)/scaleY,
                    faces.at<float>(i, LEFT_EYE_X)/scaleX,
                    faces.at<float>(i, LEFT_EYE_Y)/scaleY,
                    faces.at<float>(i, RIGHT_EYE_X)/scaleX,
                    faces.at<float>(i, RIGHT_EYE_Y)/scaleY,
                    faces.at<float>(i, NOSE_TIP_X)/scaleX,
                    faces.at<float>(i, NOSE_TIP_Y)/scaleY,
                    (faces.at<float>(i, LEFT_MOUSE_X)/scaleX + faces.at<float>(i, RIGHT_MOUSE_X)/scaleX)/2,
                    (faces.at<float>(i, LEFT_MOUSE_Y)/scaleY + faces.at<float>(i, RIGHT_MOUSE_Y)/scaleY)/2,
                    0, 0, 0, 0);


//            std::cout << "Face " << i
//                        << ", top-left coordinates: (" << faces.at<float>(i, 0) << ", " << faces.at<float>(i, 1) << "), "
//                        << "box width: " << faces.at<float>(i, 2)  << ", box height: " << faces.at<float>(i, 3) << ", "
//                        << "score: " << faces.at<float>(i, 14) << "\n";
        }
}

t_aif_status YuNetFaceDetectorV2::fillInputTensor(const cv::Mat &img)
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv image!!");
        }

        if (m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter not initialized!!");
        }
        m_scaleSize = img.size();

        int height = m_modelInfo.height;
        int width = m_modelInfo.width;
        int channels = m_modelInfo.channels;

        cv::Mat input, blob;
        if (needPadding(img)) {
            input = getPaddedImage(img, cv::Size(width, height));
            blob = blobFromImage(input, cv::Size(width, height));
            m_paddedSize = input.size();
        } else {
            blob = blobFromImage(img, cv::Size(width, height));
            m_paddedSize = img.size();
        }
        if (blob.empty()) {
            Logi("can't get input_blob image");
            return kAifError;
        }
        blob = blob.reshape(1, 1);

        float* inputTensor = m_interpreter->typed_input_tensor<float>(0);
        std::memcpy(inputTensor, blob.data, blob.total()*blob.channels()*sizeof(float));

        return kAifOk;
    } catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }
}

void YuNetFaceDetectorV2::generatePriors()
{
    // Calculate shapes of different scales according to the shape of input image
    cv::Size feature_map_2nd = {
        int(int((m_modelInfo.width+1)/2)/2), int(int((m_modelInfo.height+1)/2)/2)
    };
    cv::Size feature_map_3rd = {
        int(feature_map_2nd.width/2), int(feature_map_2nd.height/2)
    };
    cv::Size feature_map_4th = {
        int(feature_map_3rd.width/2), int(feature_map_3rd.height/2)
    };
    cv::Size feature_map_5th = {
        int(feature_map_4th.width/2), int(feature_map_4th.height/2)
    };
    cv::Size feature_map_6th = {
        int(feature_map_5th.width/2), int(feature_map_5th.height/2)
    };

    std::vector<cv::Size> feature_map_sizes;
    feature_map_sizes.push_back(feature_map_3rd);
    feature_map_sizes.push_back(feature_map_4th);
    feature_map_sizes.push_back(feature_map_5th);
    feature_map_sizes.push_back(feature_map_6th);

    // Fixed params for generating m_priors
    const std::vector<std::vector<float>> min_sizes = {
        {10.0f,  16.0f,  24.0f},
        {32.0f,  48.0f},
        {64.0f,  96.0f},
        {128.0f, 192.0f, 256.0f}
    };
    const std::vector<int> steps = { 8, 16, 32, 64 };

    // Generate m_priors
    m_priors.clear();
    for (size_t i = 0; i < feature_map_sizes.size(); ++i)
    {
        cv::Size feature_map_size = feature_map_sizes[i];
        std::vector<float> min_size = min_sizes[i];

        for (int _h = 0; _h < feature_map_size.height; ++_h)
        {
            for (int _w = 0; _w < feature_map_size.width; ++_w)
            {
                for (size_t j = 0; j < min_size.size(); ++j)
                {
                    float s_kx = min_size[j] / m_modelInfo.width;
                    float s_ky = min_size[j] / m_modelInfo.height;

                    float cx = (_w + 0.5f) * steps[i] / m_modelInfo.width;
                    float cy = (_h + 0.5f) * steps[i] / m_modelInfo.height;

                    cv::Rect2f prior = { cx, cy, s_kx, s_ky };
                    m_priors.push_back(prior);
                }
            }
        }
    }
}

bool YuNetFaceDetectorV2::needPadding(const cv::Mat& input)
{
    cv::Size input_size = input.size();
    auto input_gcd = gcd(input_size.width, input_size.height);
    auto model_gcd = gcd(m_modelInfo.width, m_modelInfo.height);

    return (input_size.width/input_gcd != m_modelInfo.width/model_gcd ||
        input_size.height/input_gcd != m_modelInfo.height/model_gcd);
}

cv::Mat YuNetFaceDetectorV2::getPaddedImage(const cv::Mat& src, const cv::Size& modelSize)
{
    cv::Mat dst;
    int srcW = src.size().width;
    int srcH = src.size().height;
    int modelW = modelSize.width;
    int modelH = modelSize.height;
    int dstW = 0;
    int dstH = 0;

    double srcRatio = static_cast<double>(srcW) / srcH;
    double modelRatio = static_cast<double>(modelW) / modelH;

    int n = (srcW / modelW) + ((srcW % modelW) != 0 ? 1 : 0);
    n = (n == 0) ? 1 : n;
    int m = (srcH / modelH) + ((srcH % modelH) != 0 ? 1 : 0);
    m = (m == 0) ? 1 : m;
    n = std::max(n, m);

    dstW = modelW * n;
    dstH = modelH * n;
    // std::cout << "model.size(" << modelW << ", " << modelH << ") n=" << n << std::endl;
    // std::cout << "padding: (" << srcW << ", " << srcH << ") => (" << dstW << ", " << dstH << ")\n";

    cv::copyMakeBorder(src, dst, 0, (dstH-srcH), 0, (dstW-srcW), cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    return dst;
}

cv::Mat YuNetFaceDetectorV2::blobFromImage(const cv::Mat& src, const cv::Size& size)
{
    cv::Mat blob;
    cv::cvtColor(src, blob, cv::COLOR_BGR2RGB);
    cv::resize(blob, blob, size, 0, 0, cv::INTER_LINEAR);
    blob.convertTo(blob, CV_32F);
    return blob;
}

unsigned YuNetFaceDetectorV2::gcd(unsigned a, unsigned b)
{
    while (b)  {
        unsigned t = b;
        b = a % b;
        a = t;
    }
    return a;
}


} // end of namespace aif
