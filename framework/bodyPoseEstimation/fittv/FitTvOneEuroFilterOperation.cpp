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
, m_initFilter(false)
, m_prevTime(-1)
, m_oneEuroParam()
, m_lowPassParam()
{
}

FitTvOneEuroFilterOperation::~FitTvOneEuroFilterOperation()
{
}

bool FitTvOneEuroFilterOperation::runImpl(const std::shared_ptr<NodeInput>& input)
{
    Logd("Using One Euro Filter!!!");
    Stopwatch sw;
    sw.start();

    auto& descriptor = input->getDescriptor();
    if (!descriptor) {
        Loge(m_id, ": failed to run operaiton (descriptor is null)");
        return false;
    }

    std::shared_ptr<FitTvPoseDescriptor> fdescriptor = std::dynamic_pointer_cast<FitTvPoseDescriptor>(input->getDescriptor());
    if (fdescriptor == nullptr) {
        Loge(__func__, "failed to convert Descriptor to FitTvPoseDescriptor");
        return false;
    }

    if((fdescriptor->getKeyPoints()).size() == 0) {
        Loge(m_id, ": failed to get Pose2d Keypoints from FitTvPoseDescriptor");
        return false;
    }

    std::shared_ptr<FitTvOneEuroFilterOperationConfig> config = std::dynamic_pointer_cast<FitTvOneEuroFilterOperationConfig>(m_config);
    if (config == nullptr) {
        Loge(__func__, "failed to convert OperationConfig to FitTvOneEuroFilterOperationConfig");
        return false;
    }
    float init_freq = config->getFilterFreq(); // freq
    float dcutoff = config->getFilterDCutOff();
    float mincutoff = config->getFilterMinCutOff();
    float beta = config->getFilterBeta();

    if(!m_initFilter) {
        for(int i=0; i<DEFAULT_NUM_KEYPOINTS; i++) { // 41
            for(int j=0; j<2; j++) { // 2 :X and Y
                m_oneEuroParam[i][j][0] = init_freq;
                m_oneEuroParam[i][j][1] = -1; // last_timestamp

                m_lowPassParam[i][j][0][0] = 0; // dxfilter - hatxprev
                m_lowPassParam[i][j][1][0] = 0; // dxfilter - xprev
                m_lowPassParam[i][j][2][0] = false; // dxfilter - hadprev

                m_lowPassParam[i][j][0][1] = 0; // xfilter - hatxprev
                m_lowPassParam[i][j][1][1] = 0; // xfilter - xprev
                m_lowPassParam[i][j][2][1] = false; // xfilter - hadprev
            }
        }
        m_initFilter = true;
    }

    std::vector<std::vector<std::vector<float>>> updatedKeypoints;
    for (auto& keyPoints : fdescriptor->getKeyPoints()) {
        std::vector<std::vector<float>> updated_pairXY;
        int idx = 0;
        double cur_time = sw.getMs();

        if(std::abs(m_prevTime + 1.f) < aif::EPSILON) m_prevTime = cur_time; // m_prevTime == -1
        else {
            m_prevTime += cur_time;
            cur_time = m_prevTime;
        }

        for (auto& keyPoint : keyPoints) {
            if (keyPoint[0] < 0) { // for invalid keypoint, skip it.
                updated_pairXY.push_back( keyPoint );
                idx++;
                continue;
            }

            float updatedX = oneEuroFilter(idx, 0, keyPoint[1], cur_time, mincutoff, dcutoff, beta); // x
            float updatedY = oneEuroFilter(idx, 1, keyPoint[2], cur_time, mincutoff, dcutoff, beta); // y
            idx++;
            updated_pairXY.push_back({keyPoint[0], updatedX, updatedY});
        }
        updatedKeypoints.push_back(updated_pairXY);
    }
    if (!(fdescriptor->updateKeyPoints(updatedKeypoints))) return false;

    sw.stop();
    return true;
}

float FitTvOneEuroFilterOperation::lowPassFilter(int idx, int is_y, int is_xflit, float x, float alpha)
{
    if (idx < 0 || idx >= DEFAULT_NUM_KEYPOINTS) {
        Loge(__func__, "Loss Pass Filter Idx is out of range: ", idx);
        return 0;
    }
    if (is_y < 0 || is_y >= DEFUALT_XY) {
        Loge(__func__, "Loss Pass Filter is_y is out of range: ", is_y);
        return 0;
    }
    if (is_xflit < 0 || is_xflit >= DEFAULT_LOW_PASS_FILTER_XDX) {
        Loge(__func__, "Loss Pass Filter is_xflit is out of range: ", is_xflit);
        return 0;
    }

    float hatxprev = m_lowPassParam[idx][is_y][0][is_xflit];
    float xprev    = m_lowPassParam[idx][is_y][1][is_xflit];
    float hadprev   = m_lowPassParam[idx][is_y][2][is_xflit];
    float hatx;

    if(std::abs(hadprev - 1.f) < aif::EPSILON) { // hadprev == 1
        hatx = alpha * x + (1-alpha) * hatxprev;
    } else {
        hatx = x;
        m_lowPassParam[idx][is_y][2][is_xflit] = true; // update hadprev
    }

    m_lowPassParam[idx][is_y][0][is_xflit] = hatx; // update hatxprev
    m_lowPassParam[idx][is_y][1][is_xflit] = x; // update xprev
    return hatx;
}

float FitTvOneEuroFilterOperation::oneEuroFilter(int idx, int is_y, float x, float t, float mincutoff, float dcutoff, float beta)
{
    if (idx < 0 || idx >= DEFAULT_NUM_KEYPOINTS) {
        Loge(__func__, "Loss Pass Filter Idx is out of range: ", idx);
        return 0;
    }
    if (is_y < 0 || is_y >= DEFUALT_XY) {
        Loge(__func__, "Loss Pass Filter is_y is out of range: ", is_y);
        return 0;
    }

    float freq     = m_oneEuroParam[idx][is_y][0];
    float lasttime = m_oneEuroParam[idx][is_y][1];
    float dx = 0;

    if (!(std::abs(lasttime + 1.f) < aif::EPSILON) && !(std::abs(t + 1.f) < aif::EPSILON) && !(std::abs(t - lasttime) < aif::EPSILON)) { // lasttime != -1 && t != -1 && t != lasttime
        freq = 1.0 / (t - lasttime);
        m_oneEuroParam[idx][is_y][0] = freq; //update frequency (average 200)
    }
    m_oneEuroParam[idx][is_y][1] = t; // update last timestamp

    if(std::abs(m_lowPassParam[idx][is_y][2][1] - 1.f) < aif::EPSILON) { // (xfilt's hadprev) == 1
        dx = (x - m_lowPassParam[idx][is_y][1][1]) * freq;
    }

    float edx    = lowPassFilter(idx, is_y, 0, dx, calAlpha(dcutoff, freq)); // dxfilt_
    float cutoff = mincutoff + beta * std::abs(edx);
    return lowPassFilter(idx, is_y, 1, x, calAlpha(cutoff, freq)); // xflit_
}

float FitTvOneEuroFilterOperation::calAlpha(float cutoff, float freq)
{
    float tau = 1.0 / (2 * M_PI * cutoff);
    float te  = 1.0 / freq;
    return 1.0 / (1.0 + tau / te);
}

} // end of namespace aif
