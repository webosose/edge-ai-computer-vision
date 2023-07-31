/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/poseLandmark/EdgeTpuPoseLandmarkDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

EdgeTpuPoseLandmarkDetector::EdgeTpuPoseLandmarkDetector()
    : PoseLandmarkDetector("not_support_edgetpu") {}

EdgeTpuPoseLandmarkDetector::~EdgeTpuPoseLandmarkDetector() {
    // Releases interpreter instance before the EdgeTpuContext is destroyed.
    // the lifetime of EdgeTpuContext should be longer than all associated
    // interpreter instances.
    m_interpreter.reset();
}

t_aif_status EdgeTpuPoseLandmarkDetector::compileModel(
    tflite::ops::builtin::BuiltinOpResolver &resolver) /* override*/
{
    Logi("Compile Model: EdgeTpuPoseLandmarkDetector");

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

} // namespace aif
