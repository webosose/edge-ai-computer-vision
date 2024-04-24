/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FITTV_POSE_DESCRIPTOR_H
#define AIF_FITTV_POSE_DESCRIPTOR_H

#include <aif/pipe/PipeDescriptor.h>
#include <aif/bodyPoseEstimation/personDetect/PersonDetectDescriptor.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/bodyPoseEstimation/pose3d/Pose3dDescriptor.h>

#include <aif/bodyPoseEstimation/common.h>
#include <aif/log/Logger.h>

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
        void addCropBox (const BBox& cropBox) { m_cropBox.push_back(cropBox); }
        void addCropImage(const cv::Mat& img) { m_cropImgs.push_back(img); }
        void addCropData(const Scale& cropScale) { m_cropScales.push_back(cropScale); }
        void addPose3dInput(const cv::Mat& input) { m_pose3dInputs.push_back(input); }

        const std::vector<BBox>& getBboxes() const { return m_boxes; }
        const cv::Rect& getRoiRect() const { return m_roiRect; }
        bool isRoiValid() const { return m_roiValid; }
        const std::vector<BBox>& getCropBbox() const { return m_cropBox; }
        const std::vector<cv::Rect>& getCropRects() const { return m_cropRects; }
        const std::vector<cv::Mat>& getCropImages() const { return m_cropImgs; }
        const std::vector<Scale>& getCropData() const { return m_cropScales; }
        const std::vector<std::vector<std::vector<float>>>& getKeyPoints() const { return m_keyPoints; }
        const std::vector<cv::Mat>& getPose3dInputs() const { return m_pose3dInputs; }
        int getNumSkippedFrames() const { return m_numSkippedFrames; }
        bool updateKeyPoints(std::vector<std::vector<std::vector<float>>>& updatedKeyPoints) {
                m_keyPoints.swap(updatedKeyPoints); // update pose2d keypoints

                int index = m_trackId - 2;
                if (m_keyPoints.size() != INT_TO_ULONG(index+1)) {
                    Loge("cannot update keyPoints by trackId: ", m_trackId-1);
                    return false;
                }

                rj::Document::AllocatorType& allocator = m_root.GetAllocator();
                if (!m_root.HasMember("poseEstimation") || m_root["poseEstimation"].Size() <= 0) {
                    Loge("not exist bbox");
                    return false;
                }

                const auto& poses = m_root["poseEstimation"].GetArray();
                if (m_trackId <= 0 || poses.Size() < m_trackId-1 || !poses[index].HasMember("id")) {
                    Loge("cannot find trackId: ", m_trackId-1);
                    return false;
                }

                const auto& pose = poses[index].GetObject();
                pose.RemoveMember("joints2D");
                rj::Value pose2d(rj::kArrayType);

                for(auto& pos: m_keyPoints[index]) {
                    rj::Value keyPoint(rj::kArrayType);
                    keyPoint.PushBack(pos[1], allocator); // x
                    keyPoint.PushBack(pos[2], allocator); // y
                    keyPoint.PushBack(pos[0], allocator); // score
                    pose2d.PushBack(keyPoint, allocator);
                }
                pose.AddMember("joints2D", pose2d, allocator); // update joints2D values for final json output

                return true;
        }


   private:
        bool addCropBridgeResult(
                const std::string& nodeId,
                const std::string& result);
        bool addPersonDetectorResult(
                const std::string& id,
                const std::shared_ptr<PersonDetectDescriptor> descriptor);
        bool addPose2dDetectorResult(
                const std::string& id,
                const std::shared_ptr<Pose2dDescriptor> descriptor);
        bool addPose3dDetectorResult(
                const std::string& id,
                const std::shared_ptr<Pose3dDescriptor> descriptor);
        bool addPose3dDetectorPosResult(
                const std::string& id,
                const std::shared_ptr<Pose3dDescriptor> descriptor);
        bool addPose3dDetectorTrajResult(
                const std::string& id,
                const std::shared_ptr<Pose3dDescriptor> descriptor);

        bool addBBox(float scroe, const BBox& box, double confidenceThreshold=0.0, const std::string &dbg_fname="");
        bool addRoi(const cv::Rect& rect);
        void addNumSkippedFrames(int num) { m_numSkippedFrames = num; }
        bool addCropRect(int trackId, const cv::Rect& rect);
        bool addPose2d(int trackId, const std::vector<std::vector<float>>& keyPoints);
        bool addPose3d(int trackId,
                const std::vector<Joint3D>& keyPoints,
                const Joint3D& trajectory);
        bool clipKeypointRoiRange(std::vector<float> &pos);
        bool addPose3dPos(int trackId,
                const std::vector<Joint3D>& keyPoints);
        bool addPose3dTraj(int trackId,
                const Joint3D& trajectory);

    private:
        int m_trackId;
        int m_numSkippedFrames;
        std::vector<BBox> m_boxes;
        cv::Rect m_roiRect;
        bool m_roiValid;
        std::vector<BBox> m_cropBox;
        std::vector<cv::Rect> m_cropRects;
        std::vector<cv::Mat> m_cropImgs;
        std::vector<Scale> m_cropScales;
        std::vector<std::vector<std::vector<float>>> m_keyPoints;
        std::vector<cv::Mat> m_pose3dInputs;
};

} // end of namespace aif

#endif // AIF_FITTV_POSE_DESCRIPTOR_H
