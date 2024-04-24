/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/faceMesh/CpuFaceMeshDetector.h>
#include <aif/tools/Utils.h>

#include <stdexcept>

namespace aif {

CpuFaceMeshDetector::CpuFaceMeshDetector()
    : FaceMeshDetector("face_landmark.tflite") {
}

CpuFaceMeshDetector::~CpuFaceMeshDetector() {
}

t_aif_status CpuFaceMeshDetector::compileModel(
    tflite::ops::builtin::BuiltinOpResolver &resolver) {
    {
        Logi("Compile Model: CpuFaceMeshDetector");
        std::stringstream errlog;
        try {
            TfLiteStatus res = kTfLiteError;
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
}

} // end of namespace aif
