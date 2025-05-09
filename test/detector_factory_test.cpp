/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/Detector.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>

#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>

using namespace aif;

class TestDetector1 : public Detector {
  public:
    TestDetector1() : Detector("yunet_yunet_final_360_640_simplify_float32.tflite") {}

    t_aif_status init(const std::string &param) override { return kAifOk; }
    std::shared_ptr<DetectorParam> createParam() override {
        return std::make_shared<DetectorParam>();
    }
    t_aif_status fillInputTensor(const cv::Mat &img) override { return kAifOk; }
    t_aif_status preProcessing() { return kAifOk; }
    t_aif_status
    postProcessing(const cv::Mat &img,
                   std::shared_ptr<Descriptor> &descriptor) override {
        return kAifOk;
    }
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override {
        return kAifOk;
    }
};
DetectorFactoryRegistration<TestDetector1, Descriptor>
    test_detector1("test_detector1");

class TestDetector2 : public TestDetector1 {
  public:
    TestDetector2() {}
};
DetectorFactoryRegistration<TestDetector2, Descriptor>
    test_Detector2("test_detector2");

class DetectorFactoryTest : public ::testing::Test {
  protected:
    DetectorFactoryTest() = default;
    ~DetectorFactoryTest() = default;

    static void SetUpTestCase() {
        DetectorFactory::get().clear();
        AIVision::init();
    }

    static void TearDownTestCase() { AIVision::deinit(); }

    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(DetectorFactoryTest, 01_getDetector) {
    DetectorFactory *factory = &DetectorFactory::get();
    EXPECT_TRUE(factory != nullptr);
    EXPECT_EQ(factory->getNumDetectors(), 0);
    EXPECT_TRUE(factory->getDetector("test_detector1") != nullptr);
    EXPECT_EQ(factory->getNumDetectors(), 1);
    EXPECT_TRUE(factory->getDetector("test_detector2") != nullptr);
    EXPECT_EQ(factory->getNumDetectors(), 2);
}

TEST_F(DetectorFactoryTest, 02_clear) {
    DetectorFactory *factory = &DetectorFactory::get();
    EXPECT_TRUE(factory != nullptr);
    EXPECT_TRUE(factory->getDetector("test_detector1") != nullptr);
    EXPECT_TRUE(factory->getDetector("test_detector2") != nullptr);
    EXPECT_EQ(factory->getNumDetectors(), 2);
    factory->clear();
    EXPECT_EQ(factory->getNumDetectors(), 0);
}
