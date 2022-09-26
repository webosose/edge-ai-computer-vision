/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FITTV_POSE_DESCRIPTOR_H
#define AIF_FITTV_POSE_DESCRIPTOR_H

#include <aif/pipe/PipeDescriptor.h>
#include <aif/bodyPoseEstimation/personDetect/PersonDetectDescriptor.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/bodyPoseEstimation/Pose3d/Pose3dDescriptor.h>

namespace aif {

class FitTvPoseDescriptor : public PipeDescriptor
{
    private:
        FitTvPoseDescriptor();

    public:
        virtual ~FitTvPoseDescriptor();

        template<typename T>
            friend class PipeDescriptorFactoryRegistration;

        bool addBridgeOperationResult(
                const std::string& nodeId,
                const std::string& operationType,
                const std::string& result) override;

        bool addDetectorOperationResult(
                const std::string& nodeId,
                const std::string& model,
                const std::shared_ptr<Descriptor>& descriptor) override;

        void addCropRects(const std::vector<cv::Rect>& rects) { m_cropRects = rects; }
        void addCropImage(const cv::Mat& img) { m_cropImgs.push_back(img); }
        void addPose3dInput(const cv::Mat& input) { m_pose3dInputs.push_back(input); }

        const std::vector<BBox>& getBboxes() const { return m_boxes; }
        const std::vector<cv::Rect>& getCropRects() const { return m_cropRects; }
        const std::vector<cv::Mat>& getCropImages() const { return m_cropImgs; }
        const std::vector<std::vector<std::vector<float>>>& getKeyPoints() const { return m_keyPoints; }
        const std::vector<cv::Mat>& getPose3dInputs() const { return m_pose3dInputs; }

   private:
        bool addPersonDetectorResult(
                const std::string& id,
                const std::shared_ptr<PersonDetectDescriptor> descriptor);
        bool addPose2dDetectorResult(
                const std::string& id,
                const std::shared_ptr<Pose2dDescriptor> descriptor);
        bool addPose3dDetectorResult(
                const std::string& id,
                const std::shared_ptr<Pose3dDescriptor> descriptor);

        bool addBBox(float scroe, const BBox& box);
        bool addPose2d(int trackId, const std::vector<std::vector<float>>& keyPoints);
        bool addPose3d(int trackId,
                const std::vector<Joint3D>& keyPoints,
                const Joint3D& trajectory);

    private:
        int m_trackId;
        std::vector<BBox> m_boxes;
        std::vector<cv::Rect> m_cropRects;
        std::vector<cv::Mat> m_cropImgs;
        std::vector<std::vector<std::vector<float>>> m_keyPoints;;
        std::vector<cv::Mat> m_pose3dInputs;
};

} // end of namespace aif

#endif // AIF_FITTV_POSE_DESCRIPTOR_H
