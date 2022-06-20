/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pose/EdgeTpuPosenetDetector.h>
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

EdgeTpuPosenetDetector::EdgeTpuPosenetDetector()
    : PosenetDetector("posenet_mobilenet_v1_075_353_481_quant_decoder_edgetpu.tflite")
{
}

EdgeTpuPosenetDetector::~EdgeTpuPosenetDetector()
{
    // Releases interpreter instance before the EdgeTpuContext is destroyed.
    // the lifetime of EdgeTpuContext should be longer than all associated interpreter instances.
    m_interpreter.reset();
}

t_aif_status EdgeTpuPosenetDetector::compileModel()/* override*/
{
    Logi("Compile Model: EdgeTpuPosenetDetector");

    std::stringstream errlog;
    try {
        TfLiteStatus res = kTfLiteError;
        tflite::ops::builtin::BuiltinOpResolver resolver;
        resolver.AddCustom(coral::kPosenetDecoderOp, coral::RegisterPosenetDecoderOp());

        // Sets up the edgetpu_context. available for any 1 TPU device.
        m_edgetpuContext = edgetpu::EdgeTpuManager::GetSingleton()->OpenDevice();
        if (m_edgetpuContext == nullptr) {
            throw std::runtime_error("can't get edgetpu context!!");
        }

        // Registers Edge TPU custom op handler with Tflite resolver.
        resolver.AddCustom(edgetpu::kCustomOp, edgetpu::RegisterCustomOp());

        // Registers PosnetDecoderOp
        resolver.AddCustom(coral::kPosenetDecoderOp, coral::RegisterPosenetDecoderOp());

        res = tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter, m_param->getNumThreads());
        if (res != kTfLiteOk || m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter build failed!!");
        }

        m_interpreter->SetExternalContext(kTfLiteEdgeTpuContext, m_edgetpuContext.get());
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
