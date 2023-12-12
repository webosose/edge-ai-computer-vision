/*
* Copyright (c) 2023 LG Electronics Inc.
* SPDX-License-Identifier: Apache-2.0
*/

#include <aif/log/Logger.h>
#include <aif/rppg/CpuRppgDetector.h>
#include <aif/tools/Utils.h>

#include <stdexcept>

namespace aif {

CpuRppgDetector::CpuRppgDetector()
: RppgDetector("rPPG_cpu.tflite")
{}

CpuRppgDetector::~CpuRppgDetector()
{}

t_aif_status CpuRppgDetector::compileModel(
    tflite::ops::builtin::BuiltinOpResolver &resolver) {
    {
        Logi("Compile Model: CpuRppgDetector");
        std::stringstream errlog;
        try {
            TfLiteStatus res = kTfLiteError;
            res = tflite::InterpreterBuilder(*m_model.get(), resolver)(
                &m_interpreter, MAX_INTERPRETER_THREADS);

            if (res != kTfLiteOk || m_interpreter == nullptr) {
                throw std::runtime_error("tflite interpreter build failed!!");
            }
            return kAifOk;
        } catch (const std::exception &e) {
            Loge(__func__, "Error: ", e.what());
            return kAifError;
        } catch (...) {
            Loge(__func__, "Error: Unknown exception occured!!");
            return kAifError;
        }
    }
}
t_aif_status CpuRppgDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv matrix!!");
        }

        int batchSize = m_modelInfo.batchSize; // 1
        int height = m_modelInfo.height; //  200
        int width = m_modelInfo.width; // 2
        if (m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter not initialized!!");
        }

        // std::cout << "Checking Fill Input Tensor Input: " << std::endl;
        // for(int i=0; i< 200; i++){
        //     std::cout <<"Checking Input: " << img.at<cv::Vec2f>(0, i)[0] << ", "<< img.at<cv::Vec2f>(0, i)[1] << std::endl;
        // }

        float* inputTensor = m_interpreter->typed_input_tensor<float>(0);
        std::memcpy(inputTensor, img.ptr<float>(0), batchSize * height  * width * sizeof(float));
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status CpuRppgDetector::postProcessing(const cv::Mat &img, std::shared_ptr<Descriptor> &descriptor)
{
    try {
        const std::vector<int> &tensor_outputs = m_interpreter->outputs();
        TfLiteTensor *output = m_interpreter->tensor(tensor_outputs[0]); // [1, 200] | [1, 1, 200]
        if (output == nullptr) {
            throw std::runtime_error("can't get tflite tensor_output!!");
        }

        int out_batch = output->dims->data[0]; // 1
        int out_channel = output->dims->data[1]; // 200

        float* data= reinterpret_cast<float*>(output->data.data);
        std::shared_ptr<RppgDescriptor> rppgDescriptor = std::dynamic_pointer_cast<RppgDescriptor>(descriptor);
        if (rppgDescriptor == nullptr) {
            throw std::runtime_error("failed to convert Descriptor to RppgDescriptor");
        }
        std::vector<float> outputs;
        outputs.insert(outputs.end(), &data[0], &data[out_channel]);

        rppgDescriptor->addRppg(outputs);
        if (rppgDescriptor != nullptr) rppgDescriptor->addRppgOutput(outputs);
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }

    return kAifOk;
}

} // end of namespace aif
