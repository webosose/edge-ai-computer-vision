/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/bodypix/EdgeTpuBodypixDetector.h>
#include <aif/log/Logger.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <posenet/posenet_decoder_op.h>

#include <sstream>
#include <stdexcept>

namespace aif {

EdgeTpuBodypixDetector::EdgeTpuBodypixDetector()
    : BodypixDetector(
          "bodypix_mobilenet_v1_075_512_512_16_quant_decoder_edgetpu.tflite") {}

EdgeTpuBodypixDetector::~EdgeTpuBodypixDetector() {
    // Releases interpreter instance before the EdgeTpuContext is destroyed.
    // the lifetime of EdgeTpuContext should be longer than all associated
    // interpreter instances.
    m_interpreter.reset();
}

t_aif_status EdgeTpuBodypixDetector::compileModel(
    tflite::ops::builtin::BuiltinOpResolver &resolver) /* override*/
{
    Logi("Compile Model: EdgeTpuBodypixDetector");

    // Sets up the edgetpu_context. available for any 1 TPU device.
    m_edgetpuContext =
        edgetpu::EdgeTpuManager::GetSingleton()->OpenDevice();
    if (m_edgetpuContext == nullptr) {
        Loge("can't get edgetpu context!!");
        return kAifError;
    }

    // Registers Edge TPU custom op handler with Tflite resolver.
    resolver.AddCustom(edgetpu::kCustomOp, edgetpu::RegisterCustomOp());

    // Registers PosnetDecoderOp
    resolver.AddCustom(coral::kPosenetDecoderOp,
            coral::RegisterPosenetDecoderOp());

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
