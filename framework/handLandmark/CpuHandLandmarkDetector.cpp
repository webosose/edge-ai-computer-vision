/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/handLandmark/CpuHandLandmarkDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>

#include <stdexcept>
#include <sstream>

namespace aif {

CpuHandLandmarkDetector::CpuHandLandmarkDetector(const std::string& modelPath)
: HandLandmarkDetector(modelPath)
{
}

CpuHandLandmarkDetector::~CpuHandLandmarkDetector()
{
}

t_aif_status CpuHandLandmarkDetector::compileModel()/* override*/
{
    Logi("Compile Model: CpuHandLandmarkDetector");
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


LiteCpuHandLandmarkDetector::LiteCpuHandLandmarkDetector()
    : CpuHandLandmarkDetector("hand_landmark_lite.tflite")
{
}

FullCpuHandLandmarkDetector::FullCpuHandLandmarkDetector()
    : CpuHandLandmarkDetector("hand_landmark_full.tflite")
{
}

}
