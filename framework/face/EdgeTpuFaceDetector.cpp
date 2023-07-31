/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/face/EdgeTpuFaceDetector.h>
#include <aif/log/Logger.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <tensorflow/lite/kernels/internal/reference/non_max_suppression.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>

#include <sstream>
#include <stdexcept>

#include <edgetpu.h>

namespace {} // end of anonymous namespace

namespace aif {

//------------------------------------------------------
// EdgeTpuFaceDetector
//------------------------------------------------------
EdgeTpuFaceDetector::EdgeTpuFaceDetector(const std::string &modelPath)
    : FaceDetector(modelPath) {}

EdgeTpuFaceDetector::~EdgeTpuFaceDetector() {
    // Releases interpreter instance before the EdgeTpuContext is destroyed.
    // the lifetime of EdgeTpuContext should be longer than all associated
    // interpreter instances.
    m_interpreter.reset();
}

t_aif_status EdgeTpuFaceDetector::compileModel(
        tflite::ops::builtin::BuiltinOpResolver &resolver) {
    Logi("Compile Model: EdgeTpuFaceDetector");

    // Sets up the edgetpu_context. available for any 1 TPU device.
    m_edgetpuContext =
        edgetpu::EdgeTpuManager::GetSingleton()->OpenDevice();
    if (m_edgetpuContext == nullptr) {
        Loge("can't get edgetpu context!!");
        return kAifError;
    }

    // Registers Edge TPU custom op handler with Tflite resolver.
    resolver.AddCustom(edgetpu::kCustomOp, edgetpu::RegisterCustomOp());

    TfLiteStatus res = tflite::InterpreterBuilder(*m_model.get(), resolver)(
            &m_interpreter, MAX_INTERPRETER_THREADS);
    if (res != kTfLiteOk || m_interpreter == nullptr) {
        Loge("tflite interpreter build failed!!");
        return kAifError;
    }

    m_interpreter->SetExternalContext(kTfLiteEdgeTpuContext,
            m_edgetpuContext.get());
    return kAifOk;
}

t_aif_status
EdgeTpuFaceDetector::fillInputTensor(const cv::Mat &img) /* override*/
{
    if (img.rows == 0 || img.cols == 0) {
        Loge("invalid opencv image!!");
        return kAifError;
    }

    int height = m_modelInfo.height;
    int width = m_modelInfo.width;
    int channels = m_modelInfo.channels;

    if (m_interpreter == nullptr) {
        Loge("tflite interpreter not initialized!!");
        return kAifError;
    }

    t_aif_status res = aif::fillInputTensor<uint8_t, cv::Vec3b>(
            m_interpreter.get(), img, width, height, channels, true,
            aif::kAifNone);
    if (res != kAifOk) {
        Loge("fillInputTensor failed!!");
        return kAifError;
    }
    return res;
}

//------------------------------------------------------
// EdgeTpuShortRangeFaceDetector
//------------------------------------------------------
EdgeTpuShortRangeFaceDetector::EdgeTpuShortRangeFaceDetector()
    : EdgeTpuFaceDetector("face-detector-quantized_edgetpu.tflite") {}

EdgeTpuShortRangeFaceDetector::~EdgeTpuShortRangeFaceDetector() {}

std::shared_ptr<DetectorParam> EdgeTpuShortRangeFaceDetector::createParam() {
    std::shared_ptr<DetectorParam> param = std::make_shared<FaceParam>();
    return param;
}

} // end of namespace aif
