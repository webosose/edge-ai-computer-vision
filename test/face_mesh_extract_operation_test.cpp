/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/Pipe.h>
#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/pipe/PipeDescriptor.h>
#include <aif/faceMesh/FaceMeshExtractOperation.h>
#include <aif/faceMesh/FaceMeshDescriptor.h>
#include <aif/tools/Utils.h>
#include "ConfigUtil.h"

#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

using namespace aif;

class FaceMeshExtractOperationTest : public ::testing::Test
{
protected:
    FaceMeshExtractOperationTest() = default;
    ~FaceMeshExtractOperationTest() = default;

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
        basePath = AIVision::getBasePath();
    }

    void TearDown() override
    {
    }

    const int NUMBER_OF_LANDMARKS = 468;

public:
    std::string basePath;
    std::string faceMeshExtractPipelineTestConfig = R"(
        {
            "name": "pipe_face_mesh_test1",
            "nodes": [
                {
                    "id": "face_mesh",
                    "input": ["image"],
                    "output": ["image","inference"],
                    "operation": {
                        "type": "detector",
                        "config": {
                            "model": "facemesh_cpu",
                            "param": {
                            "autoDelegate": {"policy": "CPU_ONLY"}
                            }
                        }
                    }
                },
                {
                    "id": "extract_face_mesh",
                    "input": ["image","inference"],
                    "output": ["image","inference"],
                    "operation": {
                        "type": "face_mesh_extract",
                        "config": {"targetId": "face_mesh"}
                    }
                }
            ]
        }
    )";

    std::string faceMeshAndRGBExtractPipelineTestConfig = R"(
        {
            "name": "pipe_face_mesh_test1",
            "nodes": [
                {
                    "id": "face_mesh",
                    "input": ["image"],
                    "output": ["image","inference"],
                    "operation": {
                        "type": "detector",
                        "config": {
                            "model": "facemesh_cpu",
                            "param": {
                            "autoDelegate": {"policy": "CPU_ONLY"}
                            }
                        }
                    }
                },
                {
                    "id": "extract_face_mesh_rgb",
                    "input": ["image","inference"],
                    "output": ["image","inference"],
                    "operation": {
                        "type": "face_mesh_extract",
                        "config": {
                            "targetId": "face_mesh",
                            "rgbExtractOn": true
                        }
                    }
                }
            ]
        }
    )";
};

TEST_F(FaceMeshExtractOperationTest, face_mesh_extract_test)
{
    Pipe pipe;
    EXPECT_TRUE(pipe.build(faceMeshExtractPipelineTestConfig));
    cv::Mat image = cv::imread(basePath + "/images/mona_face.jpg");
    EXPECT_TRUE(pipe.detect(image));

    auto pipeDescriptor = pipe.getDescriptor();
    EXPECT_TRUE(pipeDescriptor != nullptr);

    const cv::Mat &afterImage = pipeDescriptor->getImage();
    EXPECT_EQ((afterImage.size() != image.size()), true);
}

TEST_F(FaceMeshExtractOperationTest, face_mesh_rgb_extract_test)
{
    Pipe pipe;
    EXPECT_TRUE(pipe.build(faceMeshAndRGBExtractPipelineTestConfig));
    cv::Mat image = cv::imread(basePath + "/images/mona_face.jpg");
    EXPECT_TRUE(pipe.detect(image));

    auto pipeDescriptor = pipe.getDescriptor();
    EXPECT_TRUE(pipeDescriptor != nullptr);

    std::string faceMeshRGBExtractResult = pipeDescriptor->getResult("extract_face_mesh_rgb");
    rj::Document dd;
    dd.Parse(faceMeshRGBExtractResult.c_str());
    EXPECT_TRUE(dd.IsObject());
    EXPECT_TRUE(dd.HasMember("meshData"));
    EXPECT_TRUE(dd["meshData"].IsArray());
    EXPECT_TRUE(dd["meshData"].Size() == 3);
}