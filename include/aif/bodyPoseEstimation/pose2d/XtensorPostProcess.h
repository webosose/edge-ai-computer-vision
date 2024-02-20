/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_XTENSOR_POST_PROCESS_H
#define AIF_XTENSOR_POST_PROCESS_H

#include <aif/base/Detector.h>
#include <aif/base/Types.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDetector.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dParam.h>
#include <aif/bodyPoseEstimation/pose2d/PostProcess.h>
#include <aif/bodyPoseEstimation/common.h>

#include <vector>
#include <opencv2/opencv.hpp>
#include <xtensor/xarray.hpp>

namespace aif {

class XtensorPostProcess : public PostProcess
{
    public:
        XtensorPostProcess(std::shared_ptr<Pose2dDetector>& detector);
        virtual ~XtensorPostProcess();

    private:
        bool processHeatMap(std::shared_ptr<Descriptor>& descriptor, xt::xarray<float>& batchHeatmaps);
        void gaussianBlur(xt::xarray<float>& batchHeatmaps, const int BLUR_KERNEL = 11) const;
        void taylor(const xt::xarray<float>& heatMap, xt::xarray<float>& coord ) const;
        void gaussianDark(xt::xarray<float>& batchHeatmaps, xt::xarray<float>& coords) const;

    protected:
        bool execute(std::shared_ptr<Descriptor>& descriptor, float* data);
};

} // end of namespace aif

#endif  // AIF_XTENSOR_POST_PROCESS_H
