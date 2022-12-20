/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/delegate/ArmNNDelegate.h>
#include <aif/tools/Utils.h>
#include <rapidjson/document.h>

#include <fstream>
namespace rj = rapidjson;

namespace aif {

ArmNNDelegate::ArmNNDelegate(const std::string& option)
    : Delegate("ArmNNDelegate", option)
    , m_delegateOptions(armnn::Compute::CpuAcc)
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
            numOptions++;
            Logi("[option ", numOptions, " ] ",
                opt.name.GetString(), " : ", optionMap[opt.name.GetString()]);
        }
        else if (opt.value.IsString()) {
            optionMap[opt.name.GetString()] = opt.value.GetString();
            numOptions++;
            Logi("[option ", numOptions, " ] ",
                opt.name.GetString(), " : ", optionMap[opt.name.GetString()]);
        } else {
            Loge("option is not string: ", opt.name.GetString());
        }
    }

    if (optionMap.find("save-cached-network") != optionMap.end() &&
        optionMap["save-cached-network"] == "true") {
        if (optionMap.find("cached-network-filepath") != optionMap.end()) {
            if (!updateCachedNetworkFile(optionMap["cached-network-filepath"])) {
                optionMap["save-cached-network"] = "false";
            }
        }
    }

    std::unique_ptr<const char*> keys =
        std::unique_ptr<const char*>(new const char*[numOptions + 1]);
    std::unique_ptr<const char*> values =
        std::unique_ptr<const char*>(new const char*[numOptions + 1]);
    int i = 0;
    for (auto& opt : optionMap) {
        Logi("armnn option: ", opt.first, " : ", opt.second);
        keys.get()[i]   = opt.first.c_str();
        values.get()[i] = opt.second.c_str();
        i++;
    }
    armnnDelegate::DelegateOptions options(
            keys.get(), values.get(), numOptions, nullptr);

    m_delegateOptions = options;
}

ArmNNDelegate::~ArmNNDelegate()
{
}

bool ArmNNDelegate::updateCachedNetworkFile(const std::string& filepath) const
{
    if (FILE *file = fopen(filepath.c_str(), "r")) {
        fclose(file);
        Logi("use cached network file: ", filepath);
        return false;
    }

    std::ofstream ofs;
    ofs.open(filepath, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    Logi("create empty cached network file: ", filepath);
    return true;
}

TfLiteDelegatePtr ArmNNDelegate::getTfLiteDelegate() const
{
    return TfLiteDelegatePtr(
            armnnDelegate::TfLiteArmnnDelegateCreate(m_delegateOptions),
            armnnDelegate::TfLiteArmnnDelegateDelete);
}

const armnnDelegate::DelegateOptions& ArmNNDelegate::getDelegateOptions() const
{
    return m_delegateOptions;
}

} // end of namespace aif
