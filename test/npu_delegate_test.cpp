/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DelegateFactory.h>
#include <aif/delegate/NpuDelegate.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <algorithm>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>

using namespace aif;

class NpuDelegateTest : public ::testing::Test {
  protected:
    NpuDelegateTest() = default;
    ~NpuDelegateTest() = default;

    static void SetUpTestCase() { AIVision::init(); }

    static void TearDownTestCase() { AIVision::deinit(); }

    void SetUp() override { DelegateFactory::get().clear(); }

    void TearDown() override {}

    std::string options {
            "{"
            "  \"min_freq\": \"30\""
            "}"
    };

};

TEST_F(NpuDelegateTest, 01_constructor) {
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("npu_delegate");
    ASSERT_TRUE(delegate.get() != nullptr);
    DelegateFactory::get().clear();
}

TEST_F(NpuDelegateTest, 02_getTfLiteDelegate) {
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("npu_delegate");
    ASSERT_TRUE(delegate.get() != nullptr);
    auto ptr = delegate->getTfLiteDelegate();
    ASSERT_TRUE(ptr.get() != nullptr);
    DelegateFactory::get().clear();
}

TEST_F(NpuDelegateTest, 03_getTfLiteDelegateWithOptions) {
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("npu_delegate", options);
    ASSERT_TRUE(delegate.get() != nullptr);

    std::shared_ptr<NpuDelegate> npuDelegate =
        std::dynamic_pointer_cast<NpuDelegate>(delegate);
    auto option = npuDelegate->getDelegateOptions();
    EXPECT_EQ(option.getMinRequestedFreq(), 30);

    DelegateFactory::get().clear();
}

