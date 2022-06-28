/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/delegate/GpuDelegate.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>
#include <rapidjson/document.h>

namespace rj = rapidjson;

namespace aif {

GpuDelegate::GpuDelegate(const std::string &option)
    : Delegate("GpuDelegate", option),
      m_delegateOptions(TfLiteGpuDelegateOptionsV2Default()) {
    setupOptions();
}

GpuDelegate::~GpuDelegate() {}

TfLiteGpuInferenceUsage
stringToInferencePreference(const std::string &preference) {
    if (preference == "FAST_SINGLE_ANSWER")
        return TFLITE_GPU_INFERENCE_PREFERENCE_FAST_SINGLE_ANSWER;
    else if (preference == "SUSTAINED_SPEED")
        return TFLITE_GPU_INFERENCE_PREFERENCE_SUSTAINED_SPEED;

    Loge("Not exist preference option: ", preference);
    return TFLITE_GPU_INFERENCE_PREFERENCE_SUSTAINED_SPEED;
}

TfLiteGpuInferencePriority stringToPriority(const std::string &priority) {
    if (priority == "AUTO")
        return TFLITE_GPU_INFERENCE_PRIORITY_AUTO;
    else if (priority == "MAX_PRECISION")
        return TFLITE_GPU_INFERENCE_PRIORITY_MAX_PRECISION;
    else if (priority == "MIN_LATENCY")
        return TFLITE_GPU_INFERENCE_PRIORITY_MIN_LATENCY;
    else if (priority == "MIN_MEMORY_USAGE")
        return TFLITE_GPU_INFERENCE_PRIORITY_MIN_MEMORY_USAGE;

    Loge("Not exist priority option: ", priority);
    return TFLITE_GPU_INFERENCE_PRIORITY_AUTO;
}

void GpuDelegate::setupOptions() {
    rj::Document payload;
    payload.Parse(m_option.c_str());
    m_delegateOptions.inference_preference =
        TFLITE_GPU_INFERENCE_PREFERENCE_FAST_SINGLE_ANSWER;

    if (payload.HasMember("inference_preference")) {
        const rj::Value &preference = payload["inference_preference"];
        m_delegateOptions.inference_preference =
            stringToInferencePreference(preference.GetString());
    }
    if (payload.HasMember("inference_priorities")) {
        const rj::Value &priorities = payload["inference_priorities"];
        if (priorities.IsArray() && priorities.Size() == 3) {
            m_delegateOptions.inference_priority1 =
                stringToPriority(priorities[0].GetString());
            m_delegateOptions.inference_priority2 =
                stringToPriority(priorities[1].GetString());
            m_delegateOptions.inference_priority3 =
                stringToPriority(priorities[2].GetString());
            Logi("inference priority1: ", priorities[0].GetString(), " : ",
                 m_delegateOptions.inference_priority1);
            Logi("inference priority2: ", priorities[1].GetString(), " : ",
                 m_delegateOptions.inference_priority2);
            Logi("inference priority3: ", priorities[2].GetString(), " : ",
                 m_delegateOptions.inference_priority3);
        } else {
            Loge("Failed to set inference priorites: ",
                 priorities[0].GetString(), priorities[1].GetString(),
                 priorities[2].GetString());
        }
    }
    if (payload.HasMember("enable_quantized_inference")) {
        if (payload["enable_quantized_inference"].GetBool()) {
            m_delegateOptions.experimental_flags |=
                TFLITE_GPU_EXPERIMENTAL_FLAGS_ENABLE_QUANT;
        } else {
            m_delegateOptions.experimental_flags &=
                ~TFLITE_GPU_EXPERIMENTAL_FLAGS_ENABLE_QUANT;
        }
        Logi("enabled quantized inference : ",
             payload["enable_quantized_inference"].GetBool());
    }
    if (payload.HasMember("backends")) {
        const rj::Value &backends = payload["backends"];
        if (backends.IsArray() && backends.Size() > 0) {
            std::string backend = backends[0].GetString();
            if (backend == "cl") {
                m_delegateOptions.experimental_flags |=
                    TFLITE_GPU_EXPERIMENTAL_FLAGS_CL_ONLY;
            } else if (backend == "gl") {
                m_delegateOptions.experimental_flags |=
                    TFLITE_GPU_EXPERIMENTAL_FLAGS_GL_ONLY;
            }
            Logi("backends : ", backends[0].GetString());
            Logi("experimental_flags : ", m_delegateOptions.experimental_flags);
        } else {
            Loge("Failed to gpu delegate option inference priorities");
        }
    }
    if (payload.HasMember("max_delegated_partitions")) {
        m_delegateOptions.max_delegated_partitions =
            payload["max_delegated_partitions"].GetInt();
    }
}

TfLiteDelegatePtr GpuDelegate::getTfLiteDelegate() const {
    return TfLiteDelegatePtr(TfLiteGpuDelegateV2Create(&m_delegateOptions),
                             &TfLiteGpuDelegateV2Delete);
}

const TfLiteGpuDelegateOptionsV2 &GpuDelegate::getDelegateOptions() const {
    return m_delegateOptions;
}

} // end of namespace aif
