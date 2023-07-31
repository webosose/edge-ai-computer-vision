/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/selfie/CpuSelfieDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <mediapipe/selfie/transpose_conv_bias.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuSelfieDetector::CpuSelfieDetector()
    : SelfieDetector("selfie_segmentation.tflite") {}

CpuSelfieDetector::~CpuSelfieDetector() {}

t_aif_status CpuSelfieDetector::compileModel(
    tflite::ops::builtin::BuiltinOpResolver &resolver) /* override*/
{
    Logi("Compile Model: CpuSelfieDetector");
    TfLiteStatus res = kTfLiteError;
    resolver.AddCustom(
            "Convolution2DTransposeBias",
            mediapipe::tflite_operations::RegisterConvolution2DTransposeBias());
    res = tflite::InterpreterBuilder(*m_model.get(), resolver)(
            &m_interpreter, MAX_INTERPRETER_THREADS);

    if (res != kTfLiteOk || m_interpreter == nullptr) {
        Loge("tflite interpreter build failed!!");
        return kAifError;
    }
    return kAifOk;
}

} // namespace aif
