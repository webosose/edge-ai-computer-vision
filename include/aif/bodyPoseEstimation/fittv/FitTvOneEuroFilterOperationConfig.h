/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FITTV_ONE_EURO_FILTER_OPERATION_CONFIG_H
#define AIF_FITTV_ONE_EURO_FILTER_OPERATION_CONFIG_H

#include <vector>

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <aif/pipe/NodeType.h>
#include <aif/pipe/PipeConfig.h>
#include <rapidjson/document.h>

#include <memory>

namespace rj = rapidjson;
namespace aif {

class FitTvOneEuroFilterOperationConfig : public BridgeOperationConfig {
    public:
        FitTvOneEuroFilterOperationConfig()
            : BridgeOperationConfig()
            , m_freq(30)
            , m_beta(0.0)
            , m_mincutoff(1.0)
            , m_dcutoff(1.0)
            {}
        virtual ~FitTvOneEuroFilterOperationConfig() {}

        bool parse(const rj::Value& value) override;
        float getFilterFreq() const { return m_freq; }
        float getFilterBeta() const { return m_beta; }
        float getFilterMinCutOff() const { return m_mincutoff; }
        float getFilterDCutOff() const { return m_dcutoff; }

    protected:
        float m_freq;
        float m_beta;
        float m_mincutoff;
        float m_dcutoff;
};

} // end of namespace aif

#endif // AIF_FITTV_ONE_EURO_FILTER_OPERATION_CONFIG_H
