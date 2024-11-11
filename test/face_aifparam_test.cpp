/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace aif;
namespace rj = rapidjson;

class FaceAifparamTest : public ::testing::Test
{
protected:
    FaceAifparamTest() = default;
    ~FaceAifparamTest() = default;
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

#if defined(USE_NNAPI) /* K24 */ // TODO: naming again!!!
    std::string face = R"(
                {
                    "model": "face_yunet_360_640",
                    "param": {
                        "commonParam": {
                            "numMaxPerson": 1
                        },
                        "modelParam": {
                            "scoreThreshold": 0.7,
                            "nmsThreshold": 0.3,
                            "topK": 5000
                        },
                        "delegates": [{
                            "name": "nnapi_delegate",
                            "option": {
                                "cache_dir": "/usr/share/aif/model_caches/",
                                "model_token": "yunet_360_640_f32_model"
                            }
                        }]
                    }
                }
                )";
#else /* default */
    std::string face = R"(
                {
                    "model": "face_yunet_360_640",
                    "param": {
                        "commonParam": {
                            "numMaxPerson": 1
                        },
                        "modelParam": {
                            "scoreThreshold": 0.7,
                            "nmsThreshold": 0.3,
                            "topK": 5000
                        },
                        "autoDelegate": {
                            "policy": "PYTORCH_MODEL_GPU",
                            "cpu_fallback_percentage": 15
                        }
                    }
                }
                )";
#endif
};

TEST_F(FaceAifparamTest, 01_generateAIFParam_face)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    auto str = ai.generateAIFParam("face_yunet");

    json j1 = json::parse(face, nullptr, false);
    json j2 = json::parse(str, nullptr, false);
    EXPECT_EQ(j1, j2);
    ai.shutdown();
}


