/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/DetectorParam.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <rapidjson/document.h>
namespace rj = rapidjson;

namespace aif {

DetectorParam::DetectorParam()
#ifdef USE_AUTO_DELEGATE
    : m_useAutoDelegate(true),
#else
    : m_useAutoDelegate(false),
#endif
      m_useXnnpack(true),
      m_numThreads(DEFAULT_NUM_THREADS){}

DetectorParam::~DetectorParam() {}

t_aif_status DetectorParam::fromJson(const std::string &param) {
    Logi("DetectorParam: ", param);
    m_param = param;
    rj::Document payload;
    payload.Parse(param.c_str());

    if (payload.HasMember("autoDelegate")) {
        m_useAutoDelegate = true;
        m_autoDelegateConfig = jsonObjectToString(payload["autoDelegate"]);
        Logi("request to use auto delegate");
    } else if (payload.HasMember("delegates")) {
        m_useAutoDelegate = false;
        m_autoDelegateConfig = "";
        Logi("request not to use auto delegate");
    }

    if (payload.HasMember("common")) {
        if (payload["common"].HasMember("useXnnpack"))
            m_useXnnpack = payload["common"]["useXnnpack"].GetBool();
        if (payload["common"].HasMember("numThreads"))
            m_numThreads = payload["common"]["numThreads"].GetInt();
        Logi("[common][useXnnpack] ", m_useXnnpack);
        Logi("[common][numThreads] ", m_numThreads);
    }

    if (!m_useAutoDelegate) {
        if (payload.HasMember("delegates")) {
            for (auto &delegate : payload["delegates"].GetArray()) {
                std::string option = "";
                if (delegate.HasMember("option"))
                    option = jsonObjectToString(delegate["option"]);
                m_delegates.push_back({delegate["name"].GetString(), option});
                Logi("[delegates][name] ", delegate["name"].GetString());
                Logi("[delegates][option] ", option);
            }
        }
    }

    return kAifOk;
}

t_aif_status DetectorParam::updateParam(const std::string& param) {
    Logi(__func__, param);

    DetectorParam new_param;
    new_param.fromJson(param);

    try {
        /* descript the params not to be updated in runtime */
        if (m_useAutoDelegate != new_param.getUseAutoDelegate()) {
            throw std::runtime_error("m_useAutoDelegate");
        }
        if (m_autoDelegateConfig != new_param.getAutoDelegateConfig()) {
            throw std::runtime_error("autoDelegateConfig");
        }
        if (m_useXnnpack != new_param.getUseXnnpack()) {
            throw std::runtime_error("m_useXnnpack");
        }
        if (m_numThreads != new_param.getNumThreads()) {
            throw std::runtime_error("m_numThreads");
        }
        if (m_delegates != new_param.getDelegates()) {
            throw std::runtime_error("m_delegates");
        }
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what(), " param can't be updated in runtime");
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }

    m_param = param;
    return kAifOk;
}

bool DetectorParam::getUseXnnpack() const { return m_useXnnpack; }

int DetectorParam::getNumThreads() const { return m_numThreads; }

const std::vector<std::pair<std::string, std::string>> &
DetectorParam::getDelegates() const {
    return m_delegates;
}

bool DetectorParam::getUseAutoDelegate() const { return m_useAutoDelegate; }

std::string DetectorParam::getAutoDelegateConfig() const {
    return m_autoDelegateConfig;
}

} // end of namespace aif
