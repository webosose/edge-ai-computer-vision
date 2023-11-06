/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_POSE2D_DETECTORV2_H
#define AIF_NPU_POSE2D_DETECTORV2_H

#include <aif/base/DetectorFactory.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDetector.h>

namespace aif {

class NpuPose2dDetectorV2 : public Pose2dDetector, public std::enable_shared_from_this<NpuPose2dDetectorV2>
{
    protected:
        NpuPose2dDetectorV2(const std::string& modelName);
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose2dDetectorV2();
        std::shared_ptr<NpuPose2dDetectorV2> get_shared_ptr() {return shared_from_this();}
    protected:
        void setModelInfo(TfLiteTensor* inputTensor) override;
        t_aif_status fillInputTensor(const cv::Mat& img) override;
        t_aif_status preProcessing() override;
        t_aif_status postProcessing(const cv::Mat& img,
                std::shared_ptr<Descriptor>& descriptor) override;
        void getInputTensorInfo(TfLiteTensor *input);
        void normalizeImageWithQuant(cv::Mat& img, cv::Mat& normImg) const;

    private:
        float mScaleIn;
        int mZeropointIn;
};

} // end of namespace aif

#endif // AIF_NPU_POSE2D_DETECTORV2_H
