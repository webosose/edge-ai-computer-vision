/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_POSE3D_DETECTORV2_H
#define AIF_NPU_POSE3D_DETECTORV2_H

#include <aif/base/DetectorFactory.h>
#include <aif/bodyPoseEstimation/pose3d/Pose3dDetector.h>
// #include <aif/bodyPoseEstimation/pose3d/Pose3dDescriptor.h>
// #include <aif/bodyPoseEstimation/pose3d/Pose3dParam.h>

#include <vector>
#include <memory>
#include <unordered_map>
#include <opencv2/opencv.hpp>

namespace aif {

class NpuPose3dDetectorV2 : public Detector {
    protected:
        NpuPose3dDetectorV2(const std::string& modelName);
    public:
        virtual ~NpuPose3dDetectorV2();

  public:
      using Joints2D = std::vector<Joint2D>;
      using JointQueue = std::deque<Joints2D>;
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
      void initializeParam();
      void getCameraIntrinsics();
      void getInputTensorInfo(TfLiteTensor *input);
      int  getOutputTensorInfo(TfLiteTensor *output);
      void fillJoints( uint8_t* inputTensorBuff );
      void fillFlippedJoints( uint8_t* inputTensorBuff );
      void postProcess_forFirstBatch(int outputIdx, TfLiteTensor* output);
      void postProcess_forSecondBatch(int outputIdx, TfLiteTensor* output);
      void averageWithFilippedResult(int idx, uint8_t* buff, uint8_t* flipped, int numInputs, int numJoints);

  private:
      int mMaxInputs;
      int mBatchSize;
      int mNumElems;
      int mNumJointsIn;
      int mNumJointsOut[RESULT_3D_IDX_MAX];
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

#endif // AIF_NPU_POSE3D_DETECTORV2_H

