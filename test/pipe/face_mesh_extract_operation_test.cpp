/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/Pipe.h>
#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/pipe/PipeDescriptor.h>
#include <aif/pipe/FaceMeshRGBExtractOperation.h>
#include <aif/pipe/FaceMaskExtractOperation.h>
#include <aif/faceMesh/FaceMeshDescriptor.h>
#include <aif/tools/Utils.h>
#include "ConfigUtil.h"

#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

using namespace aif;

class FaceMeshExtractOperationsTest : public ::testing::Test
{
protected:
    FaceMeshExtractOperationsTest() = default;
    ~FaceMeshExtractOperationsTest() = default;

    static void SetUpTestCase() {
        AIVision::init();
    }

    static void TearDownTestCase() {
        AIVision::deinit();
    }
    void SetUp() override {
        basePath = AIVision::getBasePath();
    }

    void TearDown() override {
    }

    const int NUMBER_OF_LANDMARKS = 468;

public:
    std::string basePath;
    std::string faceMeshRGBExtractPipelineTestConfig = R"(
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
                        "type": "face_mesh_rgb_extract",
                        "config": {"targetId": "face_mesh"}
                    }
                }
            ]
        }
    )";
    std::string faceMaskExtractPipelineTestConfig = R"(
        {
            "name": "pipe_face_mesh_test2",
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
                    "id": "face_mask_extract",
                    "input": ["image","inference"],
                    "output": ["image"],
                    "operation": {
                        "type": "face_mask_extract",
                        "config": {"targetId": "face_mesh"}
                    }
                }
            ]
        }
    )";
};

TEST_F(FaceMeshExtractOperationsTest, face_mesh_rgb_extract_test)
{
    Pipe pipe;
    EXPECT_TRUE(pipe.build(faceMeshRGBExtractPipelineTestConfig));
    cv::Mat image = cv::imread(basePath + "/images/mona_face.jpg");
    EXPECT_TRUE(pipe.detect(image));

    auto pipeDescriptor = pipe.getDescriptor();
    EXPECT_TRUE(pipeDescriptor != nullptr);

    std::string faceMeshResult = pipeDescriptor->getResult("face_mesh");
    rj::Document d;
    d.Parse(faceMeshResult.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("facemesh"));
    EXPECT_TRUE(d["facemesh"].IsArray());
    EXPECT_TRUE(d["facemesh"].Size() == 1);

    EXPECT_TRUE(d["facemesh"][0].HasMember("landmarks"));
    EXPECT_TRUE(d["facemesh"][0]["landmarks"].IsArray()); // [x1, y1, z1, x2, y2, z2, ...]
    EXPECT_EQ(d["facemesh"][0]["landmarks"].Size() / 3, NUMBER_OF_LANDMARKS);

    EXPECT_TRUE(d["facemesh"][0].HasMember("score"));

    EXPECT_TRUE(d["facemesh"][0].HasMember("oval"));
    EXPECT_TRUE(d["facemesh"][0]["oval"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["oval"].Size() / 2, sizeof(FACEMESH_OVAL) / sizeof(FACEMESH_OVAL[0]));

    EXPECT_TRUE(d["facemesh"][0].HasMember("leftEye"));
    EXPECT_TRUE(d["facemesh"][0]["leftEye"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["leftEye"].Size() / 2, sizeof(FACEMESH_LEFTEYE) / sizeof(FACEMESH_LEFTEYE[0]));

    EXPECT_TRUE(d["facemesh"][0].HasMember("rightEye"));
    EXPECT_TRUE(d["facemesh"][0]["rightEye"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["rightEye"].Size() / 2, sizeof(FACEMESH_RIGHTEYE) / sizeof(FACEMESH_RIGHTEYE[0]));

    EXPECT_TRUE(d["facemesh"][0].HasMember("mouth"));
    EXPECT_TRUE(d["facemesh"][0]["mouth"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["mouth"].Size() / 2, sizeof(FACEMESH_MOUTH) / sizeof(FACEMESH_MOUTH[0]));

    std::string faceMeshRGBExtractResult = pipeDescriptor->getResult("extract_face_mesh_rgb");
    rj::Document dd;
    dd.Parse(faceMeshRGBExtractResult.c_str());
    EXPECT_TRUE(dd.IsObject());
    EXPECT_TRUE(dd.HasMember("meshData"));
    EXPECT_TRUE(dd["meshData"].IsArray());
    EXPECT_TRUE(dd["meshData"].Size() == 3);
}

TEST_F(FaceMeshExtractOperationsTest, face_mask_extract_test)
{
    Pipe pipe;
    EXPECT_TRUE(pipe.build(faceMaskExtractPipelineTestConfig));
    cv::Mat image = cv::imread(basePath + "/images/mona_face.jpg");
    EXPECT_TRUE(pipe.detect(image));

    auto pipeDescriptor = pipe.getDescriptor();
    EXPECT_TRUE(pipeDescriptor != nullptr);

    std::string faceMeshResult = pipeDescriptor->getResult("face_mesh");
    rj::Document d;
    d.Parse(faceMeshResult.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("facemesh"));
    EXPECT_TRUE(d["facemesh"].IsArray());
    EXPECT_TRUE(d["facemesh"].Size() == 1);

    EXPECT_TRUE(d["facemesh"][0].HasMember("landmarks"));
    EXPECT_TRUE(d["facemesh"][0]["landmarks"].IsArray()); // [x1, y1, z1, x2, y2, z2, ...]
    EXPECT_EQ(d["facemesh"][0]["landmarks"].Size() / 3, NUMBER_OF_LANDMARKS);

    EXPECT_TRUE(d["facemesh"][0].HasMember("score"));

    EXPECT_TRUE(d["facemesh"][0].HasMember("oval"));
    EXPECT_TRUE(d["facemesh"][0]["oval"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["oval"].Size() / 2, sizeof(FACEMESH_OVAL) / sizeof(FACEMESH_OVAL[0]));

    EXPECT_TRUE(d["facemesh"][0].HasMember("leftEye"));
    EXPECT_TRUE(d["facemesh"][0]["leftEye"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["leftEye"].Size() / 2, sizeof(FACEMESH_LEFTEYE) / sizeof(FACEMESH_LEFTEYE[0]));

    EXPECT_TRUE(d["facemesh"][0].HasMember("rightEye"));
    EXPECT_TRUE(d["facemesh"][0]["rightEye"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["rightEye"].Size() / 2, sizeof(FACEMESH_RIGHTEYE) / sizeof(FACEMESH_RIGHTEYE[0]));

    EXPECT_TRUE(d["facemesh"][0].HasMember("mouth"));
    EXPECT_TRUE(d["facemesh"][0]["mouth"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["mouth"].Size() / 2, sizeof(FACEMESH_MOUTH) / sizeof(FACEMESH_MOUTH[0]));

    const cv::Mat &afterImage = pipeDescriptor->getImage();
    EXPECT_EQ((afterImage.size() != image.size()), true);
}