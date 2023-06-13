/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FITTV_ONE_EURO_FILTER_OPERATION_H
#define AIF_FITTV_ONE_EURO_FILTER_OPERATION_H

#include <aif/pipe/BridgeOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>
#include <aif/bodyPoseEstimation/fittv/FitTvOneEuroFilterOperationConfig.h>

namespace aif {

class FitTvOneEuroFilterOperation : public BridgeOperation
{
    private:
        FitTvOneEuroFilterOperation(const std::string& id);

    public:
        virtual ~FitTvOneEuroFilterOperation();
        template<typename T1, typename T2>
            friend class NodeOperationFactoryRegistration;
        bool runImpl(const std::shared_ptr<NodeInput>& input) override;
        enum {
            DEFAULT_NUM_KEYPOINTS = 41,
            DEFUALT_XY = 2,
            DEFUALT_ONE_EURO_PARAMS = 2,
            DEFAULT_LOW_PASS_FILTER_PARAMS = 3,
            DEFAULT_LOW_PASS_FILTER_XDX = 2
        };

    private:
        float lowPassFilter(int idx, int is_y, int is_xflit, float x, float alpha);
        float oneEuroFilter(int idx, int is_y, float x, float t, float mincutoff, float dcutoff, float beta);
        float calAlpha(float cutoff, float freq);

	private:
        bool m_initFilter;
        float m_prevTime;
        float m_oneEuroParam[DEFAULT_NUM_KEYPOINTS][DEFUALT_XY][DEFUALT_ONE_EURO_PARAMS];
        float m_lowPassParam[DEFAULT_NUM_KEYPOINTS][DEFUALT_XY][DEFAULT_LOW_PASS_FILTER_PARAMS][DEFAULT_LOW_PASS_FILTER_XDX];
};

NodeOperationFactoryRegistration<FitTvOneEuroFilterOperation, FitTvOneEuroFilterOperationConfig>
    fittv_oneEuro("fittv_oneEuro");

} // end of namespace aif

#endif // AIF_FITTV_ONE_EURO_FILTER_OPERATION_H