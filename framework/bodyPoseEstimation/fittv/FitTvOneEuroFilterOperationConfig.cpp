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
        m_freq = value["config"]["freq"].GetFloat();
    }
    if (value["config"].HasMember("beta")) {
        m_beta = value["config"]["beta"].GetFloat();
    }
    if (value["config"].HasMember("mincutoff")) {
        m_mincutoff = value["config"]["mincutoff"].GetFloat();
    }
    if (value["config"].HasMember("dcutoff")) {
        m_dcutoff = value["config"]["dcutoff"].GetFloat();
    }
    return true;
}

} // end of namespace aif
