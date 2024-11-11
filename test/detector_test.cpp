/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/Detector.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>

#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>

using namespace aif;

class TestDetector : public Detector {
  public:
    TestDetector() : Detector("yunet_yunet_final_360_640_simplify_float32.tflite") {
        isCalledCreateParam = false;
        isCalledCompileModel = false;
        isCalledPreProcessing = false;
        isCalledFillInputTensor = false;
        isCalledPostProcessing = false;
    }

    std::shared_ptr<DetectorParam> createParam() override {
        isCalledCreateParam = true;
        return std::make_shared<DetectorParam>();
    }
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override {
        isCalledCompileModel = true;
        tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter);
        return kAifOk;
    }
    t_aif_status fillInputTensor(const cv::Mat &img) override {
        isCalledFillInputTensor = true;
        return kAifOk;
    }
    t_aif_status preProcessing() {
        isCalledPreProcessing = true;
        return kAifOk;
        return kAifOk;
    }
    t_aif_status
    postProcessing(const cv::Mat &img,
                   std::shared_ptr<Descriptor> &descriptor) override {
        isCalledPostProcessing = true;
        return kAifOk;
    }
    bool isCalledCreateParam;
    bool isCalledCompileModel;
    bool isCalledPreProcessing;
    bool isCalledFillInputTensor;
    bool isCalledPostProcessing;

};

class DetectorTest : public ::testing::Test {
  protected:
    DetectorTest() = default;
    ~DetectorTest() = default;

    static void SetUpTestCase() { AIVision::init(); }

    static void TearDownTestCase() { AIVision::deinit(); }

    void SetUp() override { basePath = AIVision::getBasePath(); }

    void TearDown() override {}

    std::string basePath;
};

TEST_F(DetectorTest, 01_getModelName) {
    TestDetector td;
    EXPECT_EQ(td.getModelName(), "yunet_yunet_final_360_640_simplify_float32.tflite");
}

TEST_F(DetectorTest, 02_init) {
    TestDetector td;
    EXPECT_FALSE(td.isCalledCreateParam);
    EXPECT_FALSE(td.isCalledCompileModel);
    EXPECT_FALSE(td.isCalledPreProcessing);
    EXPECT_FALSE(td.isCalledFillInputTensor);
    EXPECT_FALSE(td.isCalledPostProcessing);

    ASSERT_EQ(td.init(), kAifOk);
    EXPECT_TRUE(td.isCalledCreateParam);
    EXPECT_TRUE(td.isCalledCompileModel);
    EXPECT_TRUE(td.isCalledPreProcessing);
    EXPECT_FALSE(td.isCalledFillInputTensor);
    EXPECT_FALSE(td.isCalledPostProcessing);
}

TEST_F(DetectorTest, 03_getModelInfo) {
    TestDetector td;
    ASSERT_EQ(td.init(), kAifOk);
    auto modelInfo = td.getModelInfo();
    EXPECT_EQ(modelInfo.height, 360);
    EXPECT_EQ(modelInfo.width, 640);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(DetectorTest, 04_detect) {
    TestDetector td;
    ASSERT_EQ(td.init(), kAifOk);
    EXPECT_FALSE(td.isCalledFillInputTensor);
    EXPECT_FALSE(td.isCalledPostProcessing);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Descriptor>();
    EXPECT_TRUE(td.detectFromImage(basePath + "/images/mona.jpg", descriptor) ==
                aif::kAifOk);
    EXPECT_TRUE(td.isCalledFillInputTensor);
    EXPECT_TRUE(td.isCalledPostProcessing);
}
