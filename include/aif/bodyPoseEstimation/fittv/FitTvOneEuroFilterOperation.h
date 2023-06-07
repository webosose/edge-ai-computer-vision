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

class FitTvOneEuroFilterOperation : public BridgeOperation {
    private:
        FitTvOneEuroFilterOperation(const std::string& id);

    public:
        virtual ~FitTvOneEuroFilterOperation();
        template<typename T1, typename T2>
            friend class NodeOperationFactoryRegistration;
        bool runImpl(const std::shared_ptr<NodeInput>& input) override;
        float lowPassFilter(int idx, int is_y, int is_xflit, float x, float alpha);
	    float oneEuroFilter(int idx, int is_y, float x, float t);
	    float calAlpha(float cutoff, float freq);

	private:
        bool mInitFilter;
        float mPrevTime;
        float mBeta;
        float mMincutoff;
        float mDcutoff;
	    float mOneEuroParam[42][2][2];
	    float mLowPassParam[42][2][3][2];

};

NodeOperationFactoryRegistration<FitTvOneEuroFilterOperation, FitTvOneEuroFilterOperationConfig>
    fittv_oneEuro("fittv_oneEuro");

} // end of namespace aif

#endif // AIF_FITTV_ONE_EURO_FILTER_OPERATION_H