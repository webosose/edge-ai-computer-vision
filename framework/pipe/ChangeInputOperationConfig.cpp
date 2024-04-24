/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/ChangeInputOperationConfig.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

bool ChangeInputOperationConfig::parse(const rj::Value& value)
{
    if (!BridgeOperationConfig::parse(value)) {
        return false;
    }

    if (value["config"].HasMember("changeRect") &&
        value["config"]["changeRect"].Size() == 4) {
        m_changeRect  = cv::Rect(
                value["config"]["changeRect"][0].GetInt(),
                value["config"]["changeRect"][1].GetInt(),
                value["config"]["changeRect"][2].GetInt(),
                value["config"]["changeRect"][3].GetInt());

        Logi("change rect : ", m_changeRect.x, ", "
                             , m_changeRect.y, ", "
                             , m_changeRect.width, ", "
                             , m_changeRect.height);
    }


    return true;
}

} // end of namespace aif
