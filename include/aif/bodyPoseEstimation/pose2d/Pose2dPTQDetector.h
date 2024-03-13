/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POSE2D_PTQ_DETECTOR_H
#define AIF_POSE2D_PTQ_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDetector.h>
#include <typeindex>

namespace aif {

class Pose2dPTQDetector : public Pose2dDetector, public std::enable_shared_from_this<Pose2dPTQDetector>
{
    protected:
        Pose2dPTQDetector(const std::string& modelName, std::type_index ioType);

    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Pose2dPTQDetector();
        std::shared_ptr<Pose2dPTQDetector> get_shared_ptr() {return shared_from_this();}

    protected:
        virtual void setModelInOutInfo(const std::vector<int> &t_inputs,
                               const std::vector<int> &t_outputs) override;
        virtual t_aif_status fillInputTensor(const cv::Mat& img) override;
        virtual t_aif_status preProcessing() override;
        virtual t_aif_status postProcessing(const cv::Mat& img,
                    std::shared_ptr<Descriptor>& descriptor) override;

        template <typename T>
        t_aif_status copyInputTensor(cv::Mat& normImg, size_t size_whc);
        template <typename T>
        t_aif_status copyOutputTensor(float *buffer, size_t outputSize);
        template <typename T>
        void normalizeImageWithQuant(cv::Mat& img, cv::Mat& normImg) const;

    private:
        float mScaleIn;
        int mZeropointIn;
        float mScaleOut;
        int mZeropointOut;
        std::type_index mIoType;
        TfLiteTensor *output_heatmaps;
};

} // end of namespace aif

#endif // AIF_POSE2D_PTQ_DETECTOR_H
