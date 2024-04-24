/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/pipe/PipeDescriptorFactory.h>
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>
#include <aif/bodyPoseEstimation/fittv/FitTvOneEuroFilterOperation.h>
#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Descriptor.h>

#include <gtest/gtest.h>
#include <rapidjson/document.h>

using namespace aif;
namespace rj = rapidjson;

class FitTVOneEuroFilterOperationTest : public ::testing::Test
{
protected:
    FitTVOneEuroFilterOperationTest() = default;
    ~FitTVOneEuroFilterOperationTest() = default;

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
    std::string pose2dId{"pose2d_resnet_v1_npu"};
    std::string use_one_euro_filter {
        "{"
        "  \"type\": \"fittv_oneEuro\","
        "  \"config\": {"
        "      \"freq\" : 30,"
        "      \"beta\" : 0.0,"
        "      \"mincutoff\" : 1.0,"
        "      \"dcutoff\" : 1.0"
        "  }"
        "}"
    };

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
};

TEST_F(FitTVOneEuroFilterOperationTest, 01_useOneEuroFilterConfig)
{
    rj::Document d;
    d.Parse(use_one_euro_filter.c_str());
    auto config = NodeOperationFactory::get().createConfig(d);
    auto fd = NodeOperationFactory::get().create("fittv_oneEuro", config);
    EXPECT_TRUE(fd.get() != nullptr);

    std::shared_ptr<FitTvOneEuroFilterOperationConfig> one_euro_config =
        std::dynamic_pointer_cast<FitTvOneEuroFilterOperationConfig>(config);
    EXPECT_EQ(one_euro_config->getFilterFreq(), 30);
    EXPECT_EQ(one_euro_config->getFilterBeta(), 0.0);
    EXPECT_EQ(one_euro_config->getFilterMinCutOff(), 1.0);
    EXPECT_EQ(one_euro_config->getFilterDCutOff(), 1.0);

}

TEST_F(FitTVOneEuroFilterOperationTest, 02_oneEuroFilterRunValid)
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

    rj::Document d;
    d.Parse(use_one_euro_filter.c_str());
    auto config = NodeOperationFactory::get().createConfig(d);
    auto fd_oneEuro = NodeOperationFactory::get().create("fittv_oneEuro", config);
    std::shared_ptr<FitTvOneEuroFilterOperation> oneEuroFilter = std::dynamic_pointer_cast<FitTvOneEuroFilterOperation>(fd_oneEuro);

    NodeType inputType(NodeType::INFERENCE);
    const auto& input = std::make_shared<NodeInput>("fittv_oneEuro", inputType);
    input->setDescriptor(fd);
    EXPECT_TRUE(oneEuroFilter->runImpl(input));
}
