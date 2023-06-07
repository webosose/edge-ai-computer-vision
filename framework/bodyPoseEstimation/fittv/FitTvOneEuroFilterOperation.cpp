/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/fittv/FitTvOneEuroFilterOperation.h>
#include <aif/bodyPoseEstimation/fittv/FitTvOneEuroFilterOperationConfig.h>
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>
#include <aif/tools/Stopwatch.h>

#include <vector>

namespace aif {

FitTvOneEuroFilterOperation::FitTvOneEuroFilterOperation(const std::string& id)
: BridgeOperation(id)
, mInitFilter(false)
, mPrevTime(-1)
, mBeta(0.0)
, mMincutoff(1.0)
, mDcutoff(1.0)
, mOneEuroParam()
, mLowPassParam()
{
}

FitTvOneEuroFilterOperation::~FitTvOneEuroFilterOperation()
{
}

bool FitTvOneEuroFilterOperation::runImpl(const std::shared_ptr<NodeInput>& input)
{
    Logd("Using One Euro Filter!!!");
    Stopwatch sw;
    auto& descriptor = input->getDescriptor();
    if (!descriptor) {
        Loge(m_id, ": failed to run operaiton (descriptor is null)");
        return false;
    }

    std::shared_ptr<FitTvPoseDescriptor> fdescriptor = std::dynamic_pointer_cast<FitTvPoseDescriptor>(input->getDescriptor());
    auto& new_keyPoints = const_cast<std::vector<std::vector<std::vector<float>>>&>(fdescriptor->getKeyPoints());

    if(!mInitFilter) {
        std::shared_ptr<FitTvOneEuroFilterOperationConfig> config = std::dynamic_pointer_cast<FitTvOneEuroFilterOperationConfig>(m_config);
        for(int i=0; i<41; i++){
			for(int j=0; j<2; j++){
                mOneEuroParam[i][j][0] = config->getFilterFreq(); // freq
                mOneEuroParam[i][j][1] = -1; // last_timestamp
                mBeta = config->getFilterBeta(); // beta
                mMincutoff = config->getFilterMinCutOff(); // mincutoff
                mDcutoff = config->getFilterDCutOff(); // dcutoff

                mLowPassParam[i][j][0][0] = 0; // dxfilter - hatxprev
                mLowPassParam[i][j][1][0] = 0; // dxfilter - xprev
                mLowPassParam[i][j][2][0] = false; // dxfilter - hadprev

                mLowPassParam[i][j][0][1] = 0; // xfilter - hatxprev
                mLowPassParam[i][j][1][1] = 0; // xfilter - xprev
                mLowPassParam[i][j][2][1] = false; // xfilter - hadprev
            }
        }
        mInitFilter = true;
    }

    sw.start();
    for (auto& keyPoints : new_keyPoints) {
        std::vector<float> joints2d;
        int idx = 0;
        double cur_time = sw.getMs();

        if(mPrevTime == -1) mPrevTime = cur_time;
        else {
            mPrevTime += cur_time;
            cur_time = mPrevTime;
        }

        for (auto& keyPoint : keyPoints) {
            keyPoint[1] = oneEuroFilter(idx, 0, keyPoint[1], cur_time); // x
            keyPoint[2] = oneEuroFilter(idx, 1, keyPoint[2], cur_time); // y
            idx++;
        }
    }
    return true;
}

float FitTvOneEuroFilterOperation::lowPassFilter(int idx, int is_y, int is_xflit, float x, float alpha)
{
    float hatxprev = mLowPassParam[idx][is_y][0][is_xflit];
    float xprev    = mLowPassParam[idx][is_y][1][is_xflit];
    bool hadprev   = mLowPassParam[idx][is_y][2][is_xflit];
    float hatx;

    if(hadprev) {
        hatx = alpha * x + (1-alpha) * hatxprev;
    } else {
        hatx = x;
        mLowPassParam[idx][is_y][2][is_xflit] = true; // update hadprev
    }
    mLowPassParam[idx][is_y][0][is_xflit] = hatx; // update hatxprev
    mLowPassParam[idx][is_y][1][is_xflit] = x; // update xprev
    return hatx;
}

float FitTvOneEuroFilterOperation::oneEuroFilter(int idx, int is_y, float x, float t)
{
    float freq     = mOneEuroParam[idx][is_y][0];
    float lasttime = mOneEuroParam[idx][is_y][1];
    float dx = 0;

    if(lasttime != -1 && t != -1 && t != lasttime) {
        freq = 1.0 / (t - lasttime);
        mOneEuroParam[idx][is_y][0] = freq; //update frequency (average 200)
    }

    mOneEuroParam[idx][is_y][1] = t; // update last timestamp

    if(mLowPassParam[idx][is_y][2][1]) { // xfilt - hadprev
        dx = (x - mLowPassParam[idx][is_y][1][1]) * freq;
    }

    float edx    = lowPassFilter(idx, is_y, 0, dx, calAlpha(mDcutoff, freq)); // dxfilt_
    float cutoff = mMincutoff + mBeta * std::abs(edx);
    return lowPassFilter(idx, is_y, 1, x, calAlpha(cutoff, freq)); // xflit_
}

float FitTvOneEuroFilterOperation::calAlpha(float cutoff, float freq)
{
    float tau = 1.0 / (2 * M_PI * cutoff);
    float te  = 1.0 / freq;
    return 1.0 / (1.0 + tau / te);
}

} // end of namespace aif
