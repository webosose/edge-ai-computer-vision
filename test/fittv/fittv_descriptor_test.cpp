/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Descriptor.h>
#include <aif/pipe/PipeDescriptorFactory.h>
#include <aif/base/DetectorFactory.h>
#include <gtest/gtest.h>

using namespace std;
using namespace aif;

class FitTVPoseDescriptorTest: public ::testing::Test
{
protected:
    FitTVPoseDescriptorTest() = default;
    ~FitTVPoseDescriptorTest() = default;

    static void SetUpTestCase()
    {
        AIVision::init();
    }

    static void TearDownTestCase()
    {
        AIVision::deinit();
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

#if defined(USE_O24)
    std::string personId{"person_yolov3_v2_npu"};
    std::string cropId{"fittv_person_crop"};
    std::string pose2dId{"pose2d_resnet_v2_low_npu"};
    std::string pose2d3dBridgeId{"fittv_pose2d3d"};
    std::string pose3dPosId{"pose3d_videopose3d_v2_pos_low"};
    std::string pose3dTrajId{"pose3d_videopose3d_v2_traj_low"};
#else
    std::string personId{"person_yolov3_v1_npu"};
    std::string cropId{"fittv_person_crop"};
    std::string pose2dId{"pose2d_resnet_v1_npu"};
    std::string pose2d3dBridgeId{"fittv_pose2d3d"};
    std::string pose3dId{"pose3d_videopose3d_v1_npu"};
#endif

    std::shared_ptr<PersonDetectDescriptor> makeYoloDescriptor(int numPerson)
    {
        std::shared_ptr<PersonDetectDescriptor> yd =
            std::dynamic_pointer_cast<PersonDetectDescriptor>(DetectorFactory::get().getDescriptor(personId));
        for (int i = 1; i <= numPerson; i++) {
            int x1 = i * 10; int y1 = i * 20;
            int x2 = i * 100; int y2 = i * 200;
            BBox box;
            box.addXyxy(x1, y1, x2, y2, false);
            yd->addPerson((float)i, box);
        }
        return yd;
    }

    std::vector<cv::Rect> makeCropRects(int numPerson)
    {
         std::vector<cv::Rect> rects;
         int margin = 5;
         for (int i = 1; i <= numPerson; i++) {
            int x1 = i * 10 - margin; int y1 = i * 20 - margin;
            int x2 = i * 100 + margin; int y2 = i * 200 + margin;
            cv::Rect rect(x1, y1, x2 - x1, y2 - y1);
            rects.push_back(rect);
        }
        return rects;
    }

    std::vector<std::shared_ptr<Pose2dDescriptor>> makePose2dDescriptor(int numPerson)
    {
        std::vector<std::shared_ptr<Pose2dDescriptor>> pds;
        for (int i = 1; i <= numPerson; i++) {
            std::vector<std::vector<float>> keyPoints;
            for (int j = 0; j < 41; j++) {
                keyPoints.push_back({ (float)j, i * 10.0f + j, i * 20.0f + j});
            }

            std::shared_ptr<Pose2dDescriptor> pd =
                std::dynamic_pointer_cast<Pose2dDescriptor>(DetectorFactory::get().getDescriptor(pose2dId));
            pd->setTrackId(i);
            pd->addKeyPoints(keyPoints);
            pds.emplace_back(pd);
        }
        return pds;
    }

    std::vector<cv::Mat> makeJoints2DMats(int numPerson)
    {
        std::vector<cv::Mat> joints2dMats;
        for (int i = 1; i <= numPerson; i++) {
            std::vector<float> keyPoints;
            for (int j = 0; j < 41; j++) {
                keyPoints.push_back((float)i * 10.0f + j);
                keyPoints.push_back((float)i * 20.0f + j);
            }

            cv::Mat joints2dMat(41, 2, CV_32F, keyPoints.data());
            cv::Mat joints2dMatd;
            joints2dMat.convertTo(joints2dMatd, CV_64F );
            joints2dMats.push_back(joints2dMatd);
        }
        return joints2dMats;
    }

    std::vector<std::shared_ptr<Pose3dDescriptor>> makePose3dDescriptor(int numPerson)
    {
        std::vector<std::shared_ptr<Pose3dDescriptor>> pds;
        for (int i = 1; i <= numPerson; i++) {
            std::vector<Joint3D> joints3d;
            for (int j = 0; j < 41; j++) {
                joints3d.push_back({(float)j, i * 10.0f + j, i * 20.0f + j});
            }

#if defined(USE_O24)
            std::shared_ptr<Pose3dDescriptor> pd =
                std::dynamic_pointer_cast<Pose3dDescriptor>(DetectorFactory::get().getDescriptor(pose3dPosId));
            pd->setTrackId(i);
            pd->addJoints3D(joints3d);
            pds.emplace_back(pd);

            std::shared_ptr<Pose3dDescriptor> pdTraj =
                std::dynamic_pointer_cast<Pose3dDescriptor>(DetectorFactory::get().getDescriptor(pose3dTrajId));
            pdTraj->setTrackId(i);
            pdTraj->addTraj3D({0.1f, 0.2f, 0.3f});
            pds.emplace_back(pdTraj);
#else
            std::shared_ptr<Pose3dDescriptor> pd =
                std::dynamic_pointer_cast<Pose3dDescriptor>(DetectorFactory::get().getDescriptor(pose3dId));
            pd->setTrackId(i);
            pd->addJointsAndTraj3D(joints3d, {0.1f, 0.2f, 0.3f});
            pds.emplace_back(pd);
#endif
        }
        return pds;
    }

};

TEST_F(FitTVPoseDescriptorTest, addPersonDetectorResult)
{
    auto fd = PipeDescriptorFactory::get().create("fittv_pose");
    EXPECT_TRUE(fd);

    int numPerson = 5;
    auto yd = makeYoloDescriptor(numPerson);
    EXPECT_TRUE(fd->addDetectorOperationResult("detect_person", personId, yd));

    auto json = fd->toStr();
    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poseEstimation"));
    EXPECT_TRUE(d["poseEstimation"].IsArray());
    EXPECT_TRUE(d["poseEstimation"].Size() == numPerson);

    for (int i = 1; i <= numPerson; i++) {
        int x1 = i * 10; int y1 = i * 20;
        int x2 = i * 100; int y2 = i * 200;
        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("bbox"));
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][0].GetInt(), x1); // x
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][1].GetInt(), y1); // y
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][2].GetInt(), x2 - x1); // width
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][3].GetInt(), y2 - y1); // height
        EXPECT_FLOAT_EQ(d["poseEstimation"][i-1]["bbox"][4].GetFloat(), (float)i); // score

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("id"));
        EXPECT_EQ(d["poseEstimation"][i-1]["id"].GetInt(), i);
    }

    std::cout << "Output: " << std::endl << fd->getResult() << std::endl;
}

TEST_F(FitTVPoseDescriptorTest, addCropBridgeResult)
{
    std::shared_ptr<FitTvPoseDescriptor> fd =
        std::dynamic_pointer_cast<FitTvPoseDescriptor>(PipeDescriptorFactory::get().create("fittv_pose"));
    EXPECT_TRUE(fd);

    int numPerson = 5;
    auto yd = makeYoloDescriptor(numPerson);
    EXPECT_TRUE(fd->addDetectorOperationResult("detect_person", personId, yd));

    auto rects = makeCropRects(numPerson);
    fd->addCropRects(rects);
    EXPECT_TRUE(fd->addBridgeOperationResult("person_crop", cropId, "null"));

    auto json = fd->toStr();
    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poseEstimation"));
    EXPECT_TRUE(d["poseEstimation"].IsArray());
    EXPECT_TRUE(d["poseEstimation"].Size() == numPerson);

    int margin = 5;
    for (int i = 1; i <= numPerson; i++) {
        int x1 = i * 10 - margin; int y1 = i * 20 - margin;
        int x2 = i * 100 + margin; int y2 = i * 200 + margin;
        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("cropRect"));
        EXPECT_EQ(d["poseEstimation"][i-1]["cropRect"][0].GetInt(), x1); // x
        EXPECT_EQ(d["poseEstimation"][i-1]["cropRect"][1].GetInt(), y1); // y
        EXPECT_EQ(d["poseEstimation"][i-1]["cropRect"][2].GetInt(), x2 - x1); // width
        EXPECT_EQ(d["poseEstimation"][i-1]["cropRect"][3].GetInt(), y2 - y1); // height

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("id"));
        EXPECT_EQ(d["poseEstimation"][i-1]["id"].GetInt(), i);
    }

    std::cout << "Output: " << std::endl << fd->getResult() << std::endl;
}

TEST_F(FitTVPoseDescriptorTest, addPose2dDetectorResult)
{
    auto fd = PipeDescriptorFactory::get().create("fittv_pose");
    EXPECT_TRUE(fd);

    int numPerson = 5;
    auto yd = makeYoloDescriptor(numPerson);
    EXPECT_TRUE(fd->addDetectorOperationResult("detect_person", personId, yd));

    auto pds = makePose2dDescriptor(numPerson);
    for (auto& pd : pds) {
        EXPECT_TRUE(fd->addDetectorOperationResult("detect_pose2d", pose2dId, pd));
    }

    auto json = fd->toStr();
    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poseEstimation"));
    EXPECT_TRUE(d["poseEstimation"].IsArray());
    EXPECT_TRUE(d["poseEstimation"].Size() == numPerson);

    for (int i = 1; i <= numPerson; i++) {
        int x1 = i * 10; int y1 = i * 20;
        int x2 = i * 100; int y2 = i * 200;
        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("bbox"));
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][0].GetInt(), x1); // x
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][1].GetInt(), y1); // y
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][2].GetInt(), x2 - x1); // width
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][3].GetInt(), y2 - y1); // height
        EXPECT_FLOAT_EQ(d["poseEstimation"][i-1]["bbox"][4].GetFloat(), (float)i); // score

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("id"));
        EXPECT_EQ(d["poseEstimation"][i-1]["id"].GetInt(), i);

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("joints2D"));
        EXPECT_TRUE(d["poseEstimation"][i-1]["joints2D"].IsArray());
        for (int j = 0; j < 41; j++) {
            EXPECT_EQ(d["poseEstimation"][i-1]["joints2D"][j][0].GetInt(), i * 10 + j); // x
            EXPECT_EQ(d["poseEstimation"][i-1]["joints2D"][j][1].GetInt(), i * 20 + j); // y
            EXPECT_FLOAT_EQ(d["poseEstimation"][i-1]["joints2D"][j][2].GetFloat(), (float)j); // score
        }
    }

    std::cout << "Output: " << std::endl << fd->getResult() << std::endl;
}

TEST_F(FitTVPoseDescriptorTest, addPose2dDetectorResult_invalid)
{
    auto fd = PipeDescriptorFactory::get().create("fittv_pose");
    EXPECT_TRUE(fd);

    int numPerson = 5;
    auto yd = makeYoloDescriptor(numPerson);
    EXPECT_TRUE(fd->addDetectorOperationResult("detect_person", personId, yd));

    auto pds = makePose2dDescriptor(1);
    auto pd = pds[0];
    pd->setTrackId(0);
    EXPECT_FALSE(fd->addDetectorOperationResult("detect_pose2d", pose2dId, pd));

    pd->setTrackId(6);
    EXPECT_FALSE(fd->addDetectorOperationResult("detect_pose2d", pose2dId, pd));

    pd->setTrackId(5);
    EXPECT_FALSE(fd->addDetectorOperationResult("detect_pose2d", pose2dId, pd));

    pd->setTrackId(1);
    EXPECT_TRUE(fd->addDetectorOperationResult("detect_pose2d", pose2dId, pd));

    pd->setTrackId(3);
    EXPECT_FALSE(fd->addDetectorOperationResult("detect_pose2d", pose2dId, pd));

    pd->setTrackId(2);
    EXPECT_TRUE(fd->addDetectorOperationResult("detect_pose2d", pose2dId, pd));

    std::cout << "Output: " << std::endl << fd->getResult() << std::endl;
}

TEST_F(FitTVPoseDescriptorTest, addPose2d3dBridgeResult)
{
    std::shared_ptr<FitTvPoseDescriptor> fd =
        std::dynamic_pointer_cast<FitTvPoseDescriptor>(PipeDescriptorFactory::get().create("fittv_pose"));
    EXPECT_TRUE(fd);

    int numPerson = 5;
    auto yd = makeYoloDescriptor(numPerson);
    EXPECT_TRUE(fd->addDetectorOperationResult("detect_person", personId, yd));

    auto joints2d_mats = makeJoints2DMats(numPerson);
    for (int i = 0; i < numPerson; i++) {
        fd->addPose3dInput(joints2d_mats[i]);
    }

    auto pose3dInputs = fd->getPose3dInputs();
    EXPECT_EQ(pose3dInputs.size(), numPerson);
    for (int i = 1; i <= numPerson; i++) {
        EXPECT_EQ(pose3dInputs[i-1].rows, 41);
        EXPECT_EQ(pose3dInputs[i-1].cols, 2);
    }
}

TEST_F(FitTVPoseDescriptorTest, addPose3dDetectorResult)
{
    auto fd = PipeDescriptorFactory::get().create("fittv_pose");
    EXPECT_TRUE(fd);

    int numPerson = 5;
    auto yd = makeYoloDescriptor(numPerson);
    EXPECT_TRUE(fd->addDetectorOperationResult("detect_person", personId, yd));

    auto pds = makePose2dDescriptor(numPerson);
    for (auto& pd : pds) {
        EXPECT_TRUE(fd->addDetectorOperationResult("detect_pose2d", pose2dId, pd));
    }

    auto p3ds = makePose3dDescriptor(numPerson);
#if defined(USE_O24)
    for (auto i = 0; i < p3ds.size(); i += 2) {
        EXPECT_TRUE(fd->addDetectorOperationResult("detect_pose3d_pos", pose3dPosId, p3ds[i]));
        EXPECT_TRUE(fd->addDetectorOperationResult("detect_pose3d_traj", pose3dTrajId, p3ds[i+1]));
    }
#else
    for (auto& pd : p3ds) {
        EXPECT_TRUE(fd->addDetectorOperationResult("detect_pose3d", pose3dId, pd));
    }
#endif

    auto json = fd->toStr();
    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poseEstimation"));
    EXPECT_TRUE(d["poseEstimation"].IsArray());
    EXPECT_TRUE(d["poseEstimation"].Size() == numPerson);

    for (int i = 1; i <= numPerson; i++) {
        int x1 = i * 10; int y1 = i * 20;
        int x2 = i * 100; int y2 = i * 200;
        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("bbox"));
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][0].GetInt(), x1); // x
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][1].GetInt(), y1); // y
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][2].GetInt(), x2 - x1); // width
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"][3].GetInt(), y2 - y1); // height
        EXPECT_FLOAT_EQ(d["poseEstimation"][i-1]["bbox"][4].GetFloat(), (float)i); // score

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("id"));
        EXPECT_EQ(d["poseEstimation"][i-1]["id"].GetInt(), i);

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("joints2D"));
        EXPECT_TRUE(d["poseEstimation"][i-1]["joints2D"].IsArray());
        for (int j = 0; j < 41; j++) {
            EXPECT_EQ(d["poseEstimation"][i-1]["joints2D"][j][0].GetInt(), i * 10 + j); // x
            EXPECT_EQ(d["poseEstimation"][i-1]["joints2D"][j][1].GetInt(), i * 20 + j); // y
            EXPECT_FLOAT_EQ(d["poseEstimation"][i-1]["joints2D"][j][2].GetFloat(), (float)j); // score
        }

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("joints3D"));
        EXPECT_TRUE(d["poseEstimation"][i-1]["joints3D"].IsArray());
        for (int j = 0; j < 41; j++) {
            EXPECT_TRUE(d["poseEstimation"][i-1]["joints3D"][j][0].GetFloat() - (float)j < aif::EPSILON); // x
            EXPECT_TRUE(d["poseEstimation"][i-1]["joints3D"][j][1].GetFloat() - (i * 10 + j) < aif::EPSILON); // y
            EXPECT_TRUE(d["poseEstimation"][i-1]["joints3D"][j][2].GetFloat() - (i * 20 + j) < aif::EPSILON); // z
        }
        EXPECT_TRUE(d["poseEstimation"][i-1]["joints3DPosition"][0].GetFloat() - 0.1f < aif::EPSILON); // x
        EXPECT_TRUE(d["poseEstimation"][i-1]["joints3DPosition"][1].GetFloat() - 0.2f < aif::EPSILON); // y
        EXPECT_TRUE(d["poseEstimation"][i-1]["joints3DPosition"][2].GetFloat() - 0.3f < aif::EPSILON); // z
    }

    std::cout << "Output: " << std::endl << fd->getResult() << std::endl;
}