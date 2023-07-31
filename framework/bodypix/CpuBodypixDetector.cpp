/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodypix/CpuBodypixDetector.h>
#include <aif/log/Logger.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <posenet/posenet_decoder_op.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuBodypixDetector::CpuBodypixDetector()
    : BodypixDetector(
          "bodypix_mobilenet_v1_075_512_512_16_quant_decoder.tflite") {}

CpuBodypixDetector::~CpuBodypixDetector() {}

t_aif_status CpuBodypixDetector::compileModel(
    tflite::ops::builtin::BuiltinOpResolver &resolver) /* override*/
{
    Logi("Compile Model: CpuBodypixDetector");
    TfLiteStatus res = kTfLiteError;
    resolver.AddCustom(coral::kPosenetDecoderOp,
            coral::RegisterPosenetDecoderOp());
    res = tflite::InterpreterBuilder(*m_model.get(), resolver)(
            &m_interpreter, MAX_INTERPRETER_THREADS);

    if (res != kTfLiteOk || m_interpreter == nullptr) {
        Loge("tflite interpreter build failed!!");
        return kAifError;
    }

    return kAifOk;
}

} // namespace aif
