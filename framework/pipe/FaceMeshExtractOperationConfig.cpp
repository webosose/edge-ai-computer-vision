/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/FaceMeshExtractOperationConfig.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

FaceMeshExtractOperationConfig::FaceMeshExtractOperationConfig()
: m_rgbExtract(false)
{
}

bool FaceMeshExtractOperationConfig::parse(const rj::Value& value)
{
    if (!BridgeOperationConfig::parse(value)) {
        return false;
    }

    if (value["config"].HasMember("rgbExtractOn"))
    {
        m_rgbExtract = value["config"]["rgbExtractOn"].GetBool();
        Logi("Face Mesh RGB Extract On");
    }

    return true;
}

} // end of namespace aif
