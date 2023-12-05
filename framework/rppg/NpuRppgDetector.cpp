/*
* Copyright (c) 2023 LG Electronics Inc.
* SPDX-License-Identifier: Apache-2.0
*/

#include <aif/log/Logger.h>
#include <aif/rppg/NpuRppgDetector.h>
#include <aif/tools/Utils.h>

#include <stdexcept>

namespace aif {

NpuRppgDetector::NpuRppgDetector()
: RppgDetector("rPPG_npu.tflite")
, m_inputQuatScaleIn(0.003921568859368563)
, m_outputQuatScaleIn(0.004309722688049078)
, m_zeropointIn (0.0)
{}

NpuRppgDetector::~NpuRppgDetector()
{}

t_aif_status NpuRppgDetector::compileModel(
    tflite::ops::builtin::BuiltinOpResolver &resolver) {
    {
        Logi("Compile Model: NpuRppgDetector");
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

t_aif_status NpuRppgDetector::fillInputTensor(const cv::Mat& img)/* override*/
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

        // Quantization for NPU
        cv::Mat inputQuant_img;
        inputQuant_img = img;
        inputQuant_img.convertTo(inputQuant_img, CV_8UC2);
        auto dataPtr = reinterpret_cast<float*>(img.data);
        auto quantDataPtr = reinterpret_cast<uint8_t*>(inputQuant_img.data);
        auto total = img.total();

        auto QUANT = [this](float data) {
            return static_cast<uint8_t> (std::max(std::min(((data / m_inputQuatScaleIn) + m_zeropointIn), 255.f), 0.0f)); // fixed
        };

        for(int i = 0; i < total; i++){
            float* localPtr = dataPtr + i * 2;
            uint8_t* qaunt_localPtr = quantDataPtr + i * 2;
            qaunt_localPtr[0] = QUANT(localPtr[0]);
            qaunt_localPtr[1] = QUANT(localPtr[1]);
        }

        uint8_t* inputTensor = m_interpreter->typed_input_tensor<uint8_t>(0);
        std::memcpy(inputTensor, inputQuant_img.ptr<uint8_t>(0), batchSize * height  * width * sizeof(uint8_t));
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status NpuRppgDetector::postProcessing(const cv::Mat &img, std::shared_ptr<Descriptor> &descriptor)
{
    try {
        const std::vector<int> &tensor_outputs = m_interpreter->outputs();
        TfLiteTensor *output = m_interpreter->tensor(tensor_outputs[0]); // [1, 1, 200]
        if (output == nullptr) {
            throw std::runtime_error("can't get tflite tensor_output!!");
        }

        // int out_batch = output->dims->data[0]; // 1
        int out_channel = output->dims->data[2]; // 200

        // Dequantization for NPU
        std::vector<float> outputs;
        auto outputDataPtr = reinterpret_cast<uint8_t*>(output->data.data);

        auto DEQUANT = [this](uint8_t data) {
            return static_cast<float> ((data - m_zeropointIn) * m_inputQuatScaleIn); // fixed
        };

        for(int i = 0; i < out_channel; i++){
            uint8_t* localPtr = outputDataPtr + i;
            outputs.push_back(DEQUANT(localPtr[0]));
        }

        std::shared_ptr<RppgDescriptor> rppgDescriptor = std::dynamic_pointer_cast<RppgDescriptor>(descriptor);
        if (rppgDescriptor == nullptr) {
            throw std::runtime_error("failed to convert Descriptor to RppgDescriptor");
        }
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