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

};

TEST_F(NpuDelegateTest, 01_constructor) {
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("npu_delegate");
    EXPECT_TRUE(delegate.get() != nullptr);
    DelegateFactory::get().clear();
}

TEST_F(NpuDelegateTest, 02_getTfLiteDelegate) {
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("npu_delegate");
    EXPECT_TRUE(delegate.get() != nullptr);
    auto ptr = delegate->getTfLiteDelegate();
    EXPECT_TRUE(ptr.get() != nullptr);
    DelegateFactory::get().clear();
}
