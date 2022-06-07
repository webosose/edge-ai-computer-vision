/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodypix/CpuBodypixDetector.h>
#include <aif/log/Logger.h>
#include <aif/pose/posenet_decoder_op.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

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
    std::stringstream errlog;
    try {
        TfLiteStatus res = kTfLiteError;
        resolver.AddCustom(coral::kPosenetDecoderOp,
                           coral::RegisterPosenetDecoderOp());
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

} // namespace aif
