/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/pose/CpuPosenetDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <posenet/posenet_decoder_op.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuPosenetDetector::CpuPosenetDetector()
    : PosenetDetector("posenet_mobilenet_v1_075_353_481_quant_decoder.tflite") {
}

CpuPosenetDetector::~CpuPosenetDetector() {
}

t_aif_status CpuPosenetDetector::compileModel(
        tflite::ops::builtin::BuiltinOpResolver &resolver) {
    Logi("Compile Model: CpuPosenetDetector");
    resolver.AddCustom(coral::kPosenetDecoderOp,
            coral::RegisterPosenetDecoderOp());
    TfLiteStatus res = tflite::InterpreterBuilder(*m_model.get(), resolver)(
            &m_interpreter, MAX_INTERPRETER_THREADS);

    if (res != kTfLiteOk || m_interpreter == nullptr) {
        Loge("tflite interpreter build failed!!");
        return kAifError;
    }
    return kAifOk;
}

} // namespace aif
