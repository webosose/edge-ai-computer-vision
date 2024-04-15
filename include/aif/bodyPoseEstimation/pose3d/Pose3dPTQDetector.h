/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POSE3D_PTQ_DETECTOR_H
#define AIF_POSE3D_PTQ_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/bodyPoseEstimation/pose3d/Pose3dDetector.h>
#include <typeindex>

namespace aif {

template <typename T>
class Pose3dPTQDetector : public Pose3dDetector
{
protected:
    Pose3dPTQDetector(const std::string& modelPath);

public:
    virtual ~Pose3dPTQDetector();

protected:
    virtual void setModelInOutInfo(const std::vector<int> &t_inputs,
                               const std::vector<int> &t_outputs) override;
    virtual t_aif_status fillInputTensor(const cv::Mat& img) override;
    virtual t_aif_status preProcessing() override;
    virtual t_aif_status postProcessing(const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override;
    void fillJoints(T* inputTensorBuff);
    void fillFlippedJoints(T* inputTensorBuff);
    virtual void postProcess_forFirstBatch(int outputIdx, TfLiteTensor* output) override;
    virtual void postProcess_forSecondBatch(int outputIdx, TfLiteTensor* output) override;

    template<typename U>
    void averageWithFilippedResult(int idx, T* buff, U* flipped, int numInputs, int numJoints);

private:
    T* mResultForFirstBatch[RESULT_3D_IDX_MAX];
};

} // end of namespace aif

#endif // AIF_POSE3D_PTQ_DETECTOR_H
