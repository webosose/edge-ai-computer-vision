/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CHANGE_INPUT_OPERATION_CONFIG_H
#define AIF_CHANGE_INPUT_OPERATION_CONFIG_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <aif/pipe/PipeConfig.h>
#include <rapidjson/document.h>
#include <opencv2/opencv.hpp>
#include <vector>

namespace rj = rapidjson;
namespace aif {

class ChangeInputOperationConfig : public BridgeOperationConfig {
    public:
        ChangeInputOperationConfig() : BridgeOperationConfig() {}
        virtual ~ChangeInputOperationConfig() {}

        bool parse(const rj::Value& value) override;
        const cv::Rect& getChangeRect() const { return m_changeRect; }

    private:
        cv::Rect m_changeRect;
};

} // end of namespace aif

#endif // AIF_CHANGE_INPUT_OPERATION_CONFIG_H
