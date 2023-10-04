/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POSE3D_DETECTOR_H
#define AIF_POSE3D_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/bodyPoseEstimation/Pose3d/Pose3dDescriptor.h>
#include <aif/bodyPoseEstimation/Pose3d/Pose3dParam.h>

#include <vector>
#include <memory>
#include <unordered_map>

#include <opencv2/opencv.hpp>

namespace aif {

class Pose3dDetector : public Detector
{
public:
    using Joints2D = std::vector<Joint2D>;
    using JointQueue = std::deque<Joints2D>;

    Pose3dDetector(const std::string& modelPath);
    virtual ~Pose3dDetector();

    t_aif_status detect(const cv::Mat &img,
                        std::shared_ptr<Descriptor> &descriptor) override;

protected:
    std::shared_ptr<DetectorParam> createParam() override;
    void setModelInfo(TfLiteTensor* inputTensor) override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override;

private:
    Joint2D normalizeJoints( const float x, const float y, const int width, const int height );
    t_aif_status initializeParam();
    void getCameraIntrinsics();
    t_aif_status getInputTensorInfo(TfLiteTensor *input);
    t_aif_status getOutputTensorInfo(TfLiteTensor *output, int &outputIdx);
    void fillJoints( uint8_t* inputTensorBuff );
    t_aif_status fillFlippedJoints( uint8_t* inputTensorBuff );
    void postProcess_forFirstBatch(int outputIdx, TfLiteTensor* output);
    t_aif_status postProcess_forSecondBatch(int outputIdx, TfLiteTensor* output);
    void averageWithFilippedResult(int idx, uint8_t* buff, uint8_t* flipped, int numInputs, size_t numJoints);

private:
    int mMaxInputs;
    size_t mBatchSize;
    size_t mNumElems;
    size_t mNumJointsIn;
    size_t mNumJointsOut[RESULT_3D_IDX_MAX];
    bool mIsSecondDetect;
    JointQueue mJoints_Q;
    cv::Mat mCameraMatrix;
    cv::Mat mDistCoeff;

    float mScaleIn;
    int mZeropointIn;
    float mScaleOut[RESULT_3D_IDX_MAX];
    int mZeropointOut[RESULT_3D_IDX_MAX];

    uint8_t *mResultForFirstBatch[RESULT_3D_IDX_MAX];
    std::vector<Joint3D> mResults[RESULT_3D_IDX_MAX];
};

} // end of namespace aif

#endif // AIF_POSE3D_DETECTOR_H
