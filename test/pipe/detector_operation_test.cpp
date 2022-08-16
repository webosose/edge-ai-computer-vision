/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/DetectorOperation.h>
#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include "ConfigUtil.h"

using namespace std;
using namespace aif;

class DetectorOperationTest: public ::testing::Test
{
protected:
    DetectorOperationTest() = default;
    ~DetectorOperationTest() = default;

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
        DetectorFactory::get().clear();
        basePath = AIVision::getBasePath();
    }

    void TearDown() override
    {
        basePath = AIVision::getBasePath();
    }

public:
    std::string basePath;

};

TEST_F(DetectorOperationTest, 01_face_detector_operation_test)
{
    string id = "detect_face";
    string faceConfig = R"(
        {
            "type" : "detector",
            "config": {
                "model": "face_yunet_cpu",
                "param": {
                    "autoDelegate": {
                        "policy": "CPU_ONLY"
                     }
                }
            }
        }
    )";
    std::shared_ptr<NodeOperationConfig> config =
        ConfigUtil::getOperationConfig<DetectorOperationConfig>(faceConfig);

    NodeType inputType(NodeType::IMAGE);
    NodeType outputType(NodeType::IMAGE|NodeType::INFERENCE);

    const auto& descriptor = make_shared<PipeDescriptor>();
    const auto& input = make_shared<NodeInput>(id, inputType);
    const auto& output = make_shared<NodeOutput>(id, outputType);
    const auto& operation = NodeOperationFactory::get().create(id, config);

    cv::Mat image = cv::imread(basePath + "/images/person.jpg");
    descriptor->setImage(image);
    input->setDescriptor(descriptor);

    EXPECT_TRUE(operation.get() != nullptr);
    EXPECT_TRUE(operation->run(input, output));

    string result = output->getDescriptor()->getResult(id);
    rj::Document d;
    d.Parse(result.c_str());
    EXPECT_TRUE(d["faces"].Size() == 1);
}

TEST_F(DetectorOperationTest, 02_posenet_detector_operation_test)
{
    string id = "detect_posenet";
    string poseConfig = R"(
        {
            "type" : "detector",
            "config": {
                "model": "posenet_mobilenet_cpu",
                "param": {
                    "autoDelegate": {
                        "policy": "CPU_ONLY"
                     }
                }
            }
        }
    )";
    std::shared_ptr<NodeOperationConfig> config =
        ConfigUtil::getOperationConfig<DetectorOperationConfig>(poseConfig);

    NodeType inputType(NodeType::IMAGE);
    NodeType outputType(NodeType::IMAGE|NodeType::INFERENCE);

    const auto& descriptor = make_shared<PipeDescriptor>();
    const auto& input = make_shared<NodeInput>(id, inputType);
    const auto& output = make_shared<NodeOutput>(id, outputType);
    const auto& operation = NodeOperationFactory::get().create(id, config);

    cv::Mat image = cv::imread(basePath + "/images/person.jpg");
    descriptor->setImage(image);
    input->setDescriptor(descriptor);

    EXPECT_TRUE(operation.get() != nullptr);
    EXPECT_TRUE(operation->run(input, output));

    string result = output->getDescriptor()->getResult(id);
    rj::Document d;
    d.Parse(result.c_str());
    EXPECT_TRUE(d["poses"].Size() == 1);
}
