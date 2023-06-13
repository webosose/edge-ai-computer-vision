/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/fittv/FitTvOneEuroFilterOperationConfig.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

bool FitTvOneEuroFilterOperationConfig::parse(const rj::Value& value)
{
    if (!BridgeOperationConfig::parse(value)) {
        return false;
    }

    if (value["config"].HasMember("freq")) {
        Logi("Updated freq config from defulat value :", m_freq , " to given value: ", value["config"]["freq"].GetFloat());
        m_freq = value["config"]["freq"].GetFloat();
    }
    if (value["config"].HasMember("beta")) {
        Logi("Updated beta config from defulat value :", m_beta , " to given value: ", value["config"]["beta"].GetFloat());
        m_beta = value["config"]["beta"].GetFloat();
    }
    if (value["config"].HasMember("mincutoff")) {
        Logi("Updated mincutoff config from defulat value :", m_mincutoff , " to given value: ", value["config"]["mincutoff"].GetFloat());
        m_mincutoff = value["config"]["mincutoff"].GetFloat();
    }
    if (value["config"].HasMember("dcutoff")) {
        Logi("Updated dcutoff config from defulat value :", m_dcutoff , " to given value: ", value["config"]["dcutoff"].GetFloat());
        m_dcutoff = value["config"]["dcutoff"].GetFloat();
    }
    return true;
}

} // end of namespace aif
