/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/delegate/NnapiDelegate.h>
#include <aif/tools/Utils.h>
#include <rapidjson/document.h>

#include <fstream>
#include <string>
namespace rj = rapidjson;

namespace aif {

NnapiDelegate::NnapiDelegate(const std::string& option)
    : Delegate("NnapiDelegate", option)
{
    setupOptions();
}

NnapiDelegate::~NnapiDelegate()
{
}


TfLiteDelegate* NnApiDelegateCreate(const tflite::StatefulNnApiDelegate::Options& options) {
    TfLiteDelegate* delegate = new tflite::StatefulNnApiDelegate(options);
    return delegate;
}

void NnApiDelegateDelete(TfLiteDelegate* delegate)
{
    delete delegate;
    //delete tflite::delegate::nnapi::GetDelegate(delegate);
}

void NnapiDelegate::setupOptions() {
    rj::Document payload;
    payload.Parse(m_option.c_str());

    if (payload.HasMember("cache_dir")) {
        const rj::Value &cache_dir = payload["cache_dir"];
        m_cacheDir = cache_dir.GetString();
        m_delegateOptions.cache_dir = m_cacheDir.c_str();
    }
    if (payload.HasMember("model_token")) {
        const rj::Value &model_token = payload["model_token"];
        m_modelToken = model_token.GetString();
        m_delegateOptions.model_token = m_modelToken.c_str();
    }
    if (payload.HasMember("disallow_nnapi_cpu")) {
        const rj::Value &disallow_nnapi_cpu = payload["disallow_nnapi_cpu"];
        m_delegateOptions.disallow_nnapi_cpu = disallow_nnapi_cpu.GetBool();
    }
    if (payload.HasMember("max_number_delegated_partitions")) {
        const rj::Value &maxPartition = payload["max_number_delegated_partitions"];
        m_delegateOptions.max_number_delegated_partitions = maxPartition.GetInt();
    }
    if (payload.HasMember("accelerator_name")) {
        const rj::Value &accelName = payload["accelerator_name"];
        m_acceleratorName = accelName.GetString();
        m_delegateOptions.accelerator_name = m_acceleratorName.c_str();
    }
}

TfLiteDelegatePtr NnapiDelegate::getTfLiteDelegate() const
{
    return TfLiteDelegatePtr(
            NnApiDelegateCreate(m_delegateOptions),
            NnApiDelegateDelete);
}

const tflite::StatefulNnApiDelegate::Options& NnapiDelegate::getDelegateOptions() const
{
    return m_delegateOptions;
}

} // end of namespace aif
