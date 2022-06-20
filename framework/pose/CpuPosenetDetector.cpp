/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pose/CpuPosenetDetector.h>
#include <aif/pose/posenet_decoder_op.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>
#include <tensorflow/lite/kernels/internal/reference/non_max_suppression.h>

#include <stdexcept>
#include <sstream>

namespace aif {

CpuPosenetDetector::CpuPosenetDetector()
    : PosenetDetector("posenet_mobilenet_v1_075_353_481_quant_decoder.tflite")
{
}

CpuPosenetDetector::~CpuPosenetDetector()
{
}

t_aif_status CpuPosenetDetector::compileModel()/* override*/
{
    Logi("Compile Model: CpuPosenetDetector");
    std::stringstream errlog;
    try {
        TfLiteStatus res = kTfLiteError;
        if (!m_param->getUseXnnpack()) {
            Logi("Not use xnnpack: BuiltinOpResolverWithoutDefaultDelegates");
            tflite::ops::builtin::BuiltinOpResolverWithoutDefaultDelegates resolver;
            resolver.AddCustom(coral::kPosenetDecoderOp, coral::RegisterPosenetDecoderOp());
            res = tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter, m_param->getNumThreads());
        } else {
            Logi("Use xnnpack: BuiltinOpResolver");
            tflite::ops::builtin::BuiltinOpResolver resolver;
            resolver.AddCustom(coral::kPosenetDecoderOp, coral::RegisterPosenetDecoderOp());
            res = tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter, m_param->getNumThreads());
        }
        if (res != kTfLiteOk || m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter build failed!!");
        }
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

}
