/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/DetectorParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <rapidjson/document.h>
namespace rj = rapidjson;

namespace aif {

DetectorParam::DetectorParam()
    : m_useXnnpack(false)
    , m_numThreads(DEFAULT_NUM_THREADS)
{
}

DetectorParam::~DetectorParam()
{
}

t_aif_status DetectorParam::fromJson(const std::string& param)
{
    Logi("DetectorParam: ", param);
    m_param = param;
    rj::Document payload;
    payload.Parse(param.c_str());
    if (payload.HasMember("common")) {
        if (payload["common"].HasMember("useXnnpack"))
            m_useXnnpack = payload["common"]["useXnnpack"].GetBool();
        if (payload["common"].HasMember("numThreads"))
            m_numThreads = payload["common"]["numThreads"].GetInt();
        Logi("[common][useXnnpack] ", m_useXnnpack);
        Logi("[common][numThreads] ", m_numThreads);
    }

    if (payload.HasMember("delegates")) {
        for (auto& delegate : payload["delegates"].GetArray()) {
            std::string option = jsonObjectToString(delegate["option"]);
            m_delegates.push_back({ delegate["name"].GetString(), option});
            Logi("[delegates][name] ", delegate["name"].GetString());
            Logi("[delegates][option] ", option);
        }
    }
    return kAifOk;
}

bool DetectorParam::getUseXnnpack() const
{
    return m_useXnnpack;
}

size_t DetectorParam::getNumThreads() const
{
    return m_numThreads;
}

const std::vector<std::pair<std::string, std::string>>& DetectorParam::getDelegates() const
{
    return m_delegates;
}

} // end of namespace aif
