/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>
#include <thread>

using namespace aif;
namespace rj = rapidjson;

class ApiFacadeMultiRequestTest : public ::testing::Test
{
protected:
    ApiFacadeMultiRequestTest() = default;
    ~ApiFacadeMultiRequestTest() = default;
    void SetUp() override {}
    void TearDown() override {}
};

class RequestThread {
public:
    RequestThread(
            const std::string& name,
            EdgeAIVision& ai,
            EdgeAIVision::DetectorType type,
            int count
    ) : m_name(name), m_ai(ai), m_type(type), m_count(count)
    {
        std::string basePath = AIVision::getBasePath();
        m_input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
        Logi(m_name, " request num : ", m_count);
    }
    ~RequestThread() {}

    void operator()() {
        std::string output;
        for (int i = 0; i < m_count; i++) {
            Logi(m_name, " exec: ",  i + 1);
            ASSERT_TRUE(m_ai.detect(m_type, m_input, output));
            rj::Document result;
            result.Parse(output.c_str());
            if (EdgeAIVision::DetectorType::FACE == m_type) {
                ASSERT_TRUE(result.HasMember("faces"));
                EXPECT_EQ(result["faces"].Size(), 1);
            }
            else if (EdgeAIVision::DetectorType::POSE == m_type) {
                ASSERT_TRUE(result.HasMember("poses"));
                EXPECT_EQ(result["poses"].Size(), 1);
            }
            else if (EdgeAIVision::DetectorType::SEGMENTATION == m_type) {
                ASSERT_TRUE(result.HasMember("segments"));
                EXPECT_EQ(result["segments"].Size(), 1);
            }
        }
    }
private:
    std::string m_name;
    EdgeAIVision& m_ai;
    EdgeAIVision::DetectorType m_type;
    int m_count;
    cv::Mat m_input;
};


TEST_F(ApiFacadeMultiRequestTest, 01_thread1)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    auto type = EdgeAIVision::DetectorType::FACE;

    ai.startup();
    ASSERT_TRUE(ai.isStarted());
    ASSERT_TRUE(ai.createDetector(type));

    std::thread t1(RequestThread("thread1", ai, type, 10));
    t1.join();

    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();
}

TEST_F(ApiFacadeMultiRequestTest, 02_thread2_same_model)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    auto type = EdgeAIVision::DetectorType::FACE;

    ai.startup();
    ASSERT_TRUE(ai.isStarted());
    ASSERT_TRUE(ai.createDetector(type));

    std::thread t1(RequestThread("thread1", ai, type, 5));
    std::thread t2(RequestThread("thread2", ai, type, 10));

    t1.join();
    t2.join();

    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();
}

#ifndef USE_UPDATABLE_MODELS
TEST_F(ApiFacadeMultiRequestTest, 02_thread2_different_model)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    auto type1 = EdgeAIVision::DetectorType::FACE;
    auto type2 = EdgeAIVision::DetectorType::POSE;

    ai.startup();
    ASSERT_TRUE(ai.isStarted());
    ASSERT_TRUE(ai.createDetector(type1));
    ASSERT_TRUE(ai.createDetector(type2));

    std::thread t1(RequestThread("thread1", ai, type1, 10));
    std::thread t2(RequestThread("thread2", ai, type2, 5));

    t1.join();
    t2.join();

    EXPECT_TRUE(ai.deleteDetector(type1));
    EXPECT_TRUE(ai.deleteDetector(type2));
    ai.shutdown();
}

TEST_F(ApiFacadeMultiRequestTest, 02_thread3_different_model)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    auto type1 = EdgeAIVision::DetectorType::FACE;
    auto type2 = EdgeAIVision::DetectorType::POSE;
    auto type3 = EdgeAIVision::DetectorType::SEGMENTATION;

    ai.startup();
    ASSERT_TRUE(ai.isStarted());
    ASSERT_TRUE(ai.createDetector(type1));
    ASSERT_TRUE(ai.createDetector(type2));
    ASSERT_TRUE(ai.createDetector(type3));

    std::thread t1(RequestThread("thread1", ai, type1, 10));
    std::thread t2(RequestThread("thread2", ai, type2, 5));
    std::thread t3(RequestThread("thread3", ai, type3, 7));

    t1.join();
    t2.join();
    t3.join();

    EXPECT_TRUE(ai.deleteDetector(type1));
    EXPECT_TRUE(ai.deleteDetector(type2));
    EXPECT_TRUE(ai.deleteDetector(type3));
    ai.shutdown();
}
#endif