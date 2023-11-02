/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/delegate/NpuDelegate.h>
#include <aif/tools/Utils.h>
#include <rapidjson/document.h>

#include <fstream>
namespace rj = rapidjson;

namespace aif {

NpuDelegate::NpuDelegate(const std::string& option)
    : Delegate("NpuDelegate", option)
{
    setupOptions();
}

NpuDelegate::~NpuDelegate()
{
}

void NpuDelegate::setupOptions()
{
    rj::Document payload;
    payload.Parse(m_option.c_str());

    size_t numOptions = 0;
    std::map<std::string, std::string> optionMap;
    for (auto& opt : payload.GetObject()) {
        if (opt.value.IsArray()) {
            std::string optStr;
            for (int i = 0; i < opt.value.Size(); i++) {
                if (i != 0) optStr += ", ";
                // CID 933408
                auto optValue = opt.value[i].GetString();
                if (optValue != nullptr) {
                    optStr += optValue;
                }
            }
            optionMap[opt.name.GetString()] = optStr;
        }
        else if (opt.value.IsString()) {
            optionMap[opt.name.GetString()] = opt.value.GetString();
        } else {
            Loge("option is not string: ", opt.name.GetString());
        }

        if (UINT_MAX - numOptions > 0) {
            numOptions++;
            Logi(opt.name.GetString(), " : ", optionMap[opt.name.GetString()]);
        }
    }

    if (UINT_MAX - numOptions > 0) {
        std::unique_ptr<const char*> keys =
            std::unique_ptr<const char*>(new const char*[numOptions + 1]);
        std::unique_ptr<const char*> values =
            std::unique_ptr<const char*>(new const char*[numOptions + 1]);
        int i = 0;
        for (auto& opt : optionMap) {
            Logi("npu option: ", opt.first, " : ", opt.second);
            keys.get()[i]   = opt.first.c_str();
            values.get()[i] = opt.second.c_str();
            if (INT_MAX - i > 0) i++;
        }
        webos::npu::tflite::NpuDelegateOptions options(
                keys.get(), values.get(), numOptions, nullptr);

        m_delegateOptions = options;
    }
// TODO :
}

TfLiteDelegatePtr NpuDelegate::getTfLiteDelegate() const
{
    return TfLiteDelegatePtr(
            webos::npu::tflite::TfLiteNpuDelegateCreate(m_delegateOptions),
            webos::npu::tflite::TfLiteNpuDelegateDelete);
}

const webos::npu::tflite::NpuDelegateOptions& NpuDelegate::getDelegateOptions() const
{
    return m_delegateOptions;
}

} // end of namespace aif
