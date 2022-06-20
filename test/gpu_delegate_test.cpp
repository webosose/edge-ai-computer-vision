/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DelegateFactory.h>
#include <aif/delegate/GpuDelegate.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <algorithm>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>

using namespace aif;

class GpuDelegateTest : public ::testing::Test {
  protected:
    GpuDelegateTest() = default;
    ~GpuDelegateTest() = default;

    static void SetUpTestCase() { AIVision::init(); }

    static void TearDownTestCase() { AIVision::deinit(); }

    void SetUp() override { DelegateFactory::get().clear(); }

    void TearDown() override {}

    std::string options1{"{"
                         " \"backends\": [\"cl\"],"
                         " \"inference_preference\": \"SUSTAINED_SPEED\" ,"
                         " \"inference_priorities\": [\"MIN_LATENCY\", "
                         " \"MIN_MEMORY_USAGE\", \"MAX_PRECISION\"], "
                         " \"enable_quntized_inference\" : true"
                         "}"};

    std::string options2{"{"
                         " \"backends\": [\"gl\"],"
                         " \"inference_preference\": \"FAST_SINGLE_ANSWER\" ,"
                         " \"inference_priorities\": [\"MAX_PRECISION\", "
                         " \"MIN_MEMORY_USAGE\", \"AUTO\"], "
                         " \"enable_quntized_inference\" : false"
                         "}"};
};

TEST_F(GpuDelegateTest, 01_constructor) {
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("gpu_delegate");
    EXPECT_TRUE(delegate.get() != nullptr);
    DelegateFactory::get().clear();
}

TEST_F(GpuDelegateTest, 02_constructor_with_options1) {
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("gpu_delegate", options1);
    EXPECT_TRUE(delegate.get() != nullptr);

    std::shared_ptr<GpuDelegate> gpuDelegate =
        std::dynamic_pointer_cast<GpuDelegate>(delegate);
    auto option = gpuDelegate->getDelegateOptions();
    EXPECT_EQ(option.inference_preference,
              TFLITE_GPU_INFERENCE_PREFERENCE_SUSTAINED_SPEED);
    EXPECT_EQ(option.inference_priority1,
              TFLITE_GPU_INFERENCE_PRIORITY_MIN_LATENCY);
    EXPECT_EQ(option.inference_priority2,
              TFLITE_GPU_INFERENCE_PRIORITY_MIN_MEMORY_USAGE);
    EXPECT_EQ(option.inference_priority3,
              TFLITE_GPU_INFERENCE_PRIORITY_MAX_PRECISION);
    EXPECT_EQ(option.experimental_flags &
                  TFLITE_GPU_EXPERIMENTAL_FLAGS_ENABLE_QUANT,
              TFLITE_GPU_EXPERIMENTAL_FLAGS_ENABLE_QUANT);
    EXPECT_EQ(option.experimental_flags & TFLITE_GPU_EXPERIMENTAL_FLAGS_CL_ONLY,
              TFLITE_GPU_EXPERIMENTAL_FLAGS_CL_ONLY);
    DelegateFactory::get().clear();
}

TEST_F(GpuDelegateTest, 02_constructor_with_options2) {
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("gpu_delegate", options2);
    EXPECT_TRUE(delegate.get() != nullptr);

    std::shared_ptr<GpuDelegate> gpuDelegate =
        std::dynamic_pointer_cast<GpuDelegate>(delegate);
    auto option = gpuDelegate->getDelegateOptions();
    EXPECT_EQ(option.inference_preference,
              TFLITE_GPU_INFERENCE_PREFERENCE_FAST_SINGLE_ANSWER);
    EXPECT_EQ(option.inference_priority1,
              TFLITE_GPU_INFERENCE_PRIORITY_MAX_PRECISION);
    EXPECT_EQ(option.inference_priority2,
              TFLITE_GPU_INFERENCE_PRIORITY_MIN_MEMORY_USAGE);
    EXPECT_EQ(option.inference_priority3, TFLITE_GPU_INFERENCE_PRIORITY_AUTO);
    EXPECT_EQ(option.experimental_flags &
                  TFLITE_GPU_EXPERIMENTAL_FLAGS_ENABLE_QUANT,
              0);
    DelegateFactory::get().clear();
}

TEST_F(GpuDelegateTest, 04_getTfLiteDelegate) {
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("gpu_delegate", options1);
    EXPECT_TRUE(delegate.get() != nullptr);
    auto ptr = delegate->getTfLiteDelegate();
    EXPECT_TRUE(ptr.get() != nullptr);
    DelegateFactory::get().clear();
}
