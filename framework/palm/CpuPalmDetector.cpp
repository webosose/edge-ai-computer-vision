/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/palm/CpuPalmDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>
#include <tensorflow/lite/kernels/internal/reference/non_max_suppression.h>

#include <stdexcept>
#include <sstream>

namespace aif {

CpuPalmDetector::CpuPalmDetector(const std::string& modelPath)
    : PalmDetector(modelPath)
{
}

CpuPalmDetector::~CpuPalmDetector()
{
}

t_aif_status CpuPalmDetector::compileModel()/* override*/
{
    Logi("Compile Model: CpuPalmDetector");
    std::stringstream errlog;
    try {
        TfLiteStatus res = kTfLiteError;
        if (!m_param->getUseXnnpack()) {
            Logi("Not use xnnpack: BuiltinOpResolverWithoutDefaultDelegates");
            tflite::ops::builtin::BuiltinOpResolverWithoutDefaultDelegates resolver;
            res = tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter, m_param->getNumThreads());
        } else {
            Logi("Use xnnpack: BuiltinOpResolver");
            tflite::ops::builtin::BuiltinOpResolver resolver;
            res = tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter, m_param->getNumThreads());
        }
        if (res != kTfLiteOk || m_interpreter == nullptr) {
            throw std::runtime_error("palm_detection_lite.tflite interpreter build failed!!");
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

LiteCpuPalmDetector::LiteCpuPalmDetector()
    : CpuPalmDetector("palm_detection_lite.tflite")
{
}

FullCpuPalmDetector::FullCpuPalmDetector()
    : CpuPalmDetector("palm_detection_full.tflite")
{
}

}
