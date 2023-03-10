/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_REGULAR_POST_PROCESS_H
#define AIF_REGULAR_POST_PROCESS_H

#include <aif/base/Detector.h>
#include <aif/base/Types.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDetector.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dParam.h>
#include <aif/bodyPoseEstimation/pose2d/PostProcess.h>
#include <aif/bodyPoseEstimation/common.h>

#include <vector>
#include <opencv2/opencv.hpp>

namespace aif {

class RegularPostProcess : public PostProcess
{
public:
    RegularPostProcess(std::shared_ptr<Pose2dDetector>& detector);
    virtual ~RegularPostProcess();
private:
    bool processHeatMap(std::shared_ptr<Descriptor>& descriptor, float* heatMaps);
    void gaussianBlur(float *heatMap, const int kernel) const;
    void taylor(float *heatMap, float& coord_x, float& coord_y) const;
    void gaussianDark(float *heatMap, float& coord_x, float& coord_y, const int BLUR_KERNEL = 11) const;
protected:
    bool execute(std::shared_ptr<Descriptor>& descriptor, float* data);
};

} // end of namespace aif

#endif  // AIF_REGULAR_POST_PROCESS_H
