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

    std::string personId{"person_yolov4_npu"};
    std::string pose2dId{"pose2d_resnet_npu"};
    std::string pose3dId{"pose3d_videopose3d_npu"};

    std::shared_ptr<Yolov4Descriptor> makeYoloDescriptor(int numPerson)
    {
        std::shared_ptr<Yolov4Descriptor> yd =
            std::dynamic_pointer_cast<Yolov4Descriptor>(DetectorFactory::get().getDescriptor(personId));
        for (int i = 1; i <= numPerson; i++) {
            int x1 = i * 10; int y1 = i * 20;
            int x2 = i * 100; int y2 = i * 200;
            BBox box;
            box.addXyxy(x1, y1, x2, y2, false);
            yd->addPerson((float)i, box);
        }
        return yd;
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

    std::vector<std::shared_ptr<Pose3dDescriptor>> makePose3dDescriptor(int numPerson)
    {
        std::vector<std::shared_ptr<Pose3dDescriptor>> pds;
        for (int i = 1; i <= numPerson; i++) {
            std::vector<Joint3D> joints3d;
            for (int j = 0; j < 41; j++) {
                joints3d.push_back({(float)j, i * 10.0f + j, i * 20.0f + j});
            }

            std::shared_ptr<Pose3dDescriptor> pd =
                std::dynamic_pointer_cast<Pose3dDescriptor>(DetectorFactory::get().getDescriptor(pose3dId));
            pd->setTrackId(i);
            pd->addJoints3D(joints3d, {0.1f, 0.2f, 0.3f});
            pds.emplace_back(pd);
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
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["score"].GetFloat(), (float)i);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["x"].GetInt(), x1);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["y"].GetInt(), y1);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["width"].GetInt(), x2 - x1);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["height"].GetInt(), y2 - y1);

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("trackId"));
        EXPECT_EQ(d["poseEstimation"][i-1]["trackId"].GetInt(), i);
    }
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
        EXPECT_FLOAT_EQ(d["poseEstimation"][i-1]["bbox"]["score"].GetFloat(), (float)i);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["x"].GetInt(), x1);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["y"].GetInt(), y1);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["width"].GetInt(), x2 - x1);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["height"].GetInt(), y2 - y1);

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("trackId"));
        EXPECT_EQ(d["poseEstimation"][i-1]["trackId"].GetInt(), i);

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("pose2d"));
        EXPECT_TRUE(d["poseEstimation"][i-1]["pose2d"].IsArray());
        for (int j = 0; j < 41; j++) {
            EXPECT_FLOAT_EQ(d["poseEstimation"][i-1]["pose2d"][j]["score"].GetFloat(), (float)j);
            EXPECT_EQ(d["poseEstimation"][i-1]["pose2d"][j]["x"].GetInt(), i * 10 + j);
            EXPECT_EQ(d["poseEstimation"][i-1]["pose2d"][j]["y"].GetInt(), i * 20 + j);
        }
    }
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
    for (auto& pd : p3ds) {
        EXPECT_TRUE(fd->addDetectorOperationResult("detect_pose3d", pose3dId, pd));
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
        EXPECT_FLOAT_EQ(d["poseEstimation"][i-1]["bbox"]["score"].GetFloat(), (float)i);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["x"].GetInt(), x1);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["y"].GetInt(), y1);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["width"].GetInt(), x2 - x1);
        EXPECT_EQ(d["poseEstimation"][i-1]["bbox"]["height"].GetInt(), y2 - y1);

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("trackId"));
        EXPECT_EQ(d["poseEstimation"][i-1]["trackId"].GetInt(), i);

        EXPECT_TRUE(d["poseEstimation"][i-1].HasMember("pose2d"));
        EXPECT_TRUE(d["poseEstimation"][i-1]["pose2d"].IsArray());
        for (int j = 0; j < 41; j++) {
            EXPECT_FLOAT_EQ(d["poseEstimation"][i-1]["pose2d"][j]["score"].GetFloat(), (float)j);
            EXPECT_EQ(d["poseEstimation"][i-1]["pose2d"][j]["x"].GetInt(), i * 10 + j);
            EXPECT_EQ(d["poseEstimation"][i-1]["pose2d"][j]["y"].GetInt(), i * 20 + j);
        }
        EXPECT_TRUE(d["poseEstimation"][i-1]["pose3d"].IsArray());
        for (int j = 0; j < 41; j++) {
            EXPECT_TRUE(d["poseEstimation"][i-1]["pose3d"][j]["x"].GetFloat() - (float)j < aif::EPSILON);
            EXPECT_TRUE(d["poseEstimation"][i-1]["pose3d"][j]["y"].GetFloat() - (i * 10 + j) < aif::EPSILON);
            EXPECT_TRUE(d["poseEstimation"][i-1]["pose3d"][j]["z"].GetFloat() - (i * 20 + j) < aif::EPSILON);
        }
        EXPECT_TRUE(d["poseEstimation"][i-1]["pose3dPos"]["x"].GetFloat() - 0.1f < aif::EPSILON);
        EXPECT_TRUE(d["poseEstimation"][i-1]["pose3dPos"]["y"].GetFloat() - 0.2f < aif::EPSILON);
        EXPECT_TRUE(d["poseEstimation"][i-1]["pose3dPos"]["z"].GetFloat() - 0.3f < aif::EPSILON);
    }
}
