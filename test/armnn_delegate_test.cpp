/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DelegateFactory.h>
#include <aif/delegate/ArmNNDelegate.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>


using namespace aif;

class ArmNNDelegateTest : public ::testing::Test
{
protected:
    ArmNNDelegateTest() = default;
    ~ArmNNDelegateTest() = default;


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
    }

    void TearDown() override
    {
    }

    std::string options_cpuacc_info {
            "{"
            "  \"backends\": [\"CpuAcc\"],"
            "   \"logging-severity\": \"info\""
            "}"
    };
    std::string options_cpuref_debug{
            "{"
            "  \"backends\": [\"CpuRef\"],"
            "  \"logging-severity\": \"debug\""
            "}"
    };
};

TEST_F(ArmNNDelegateTest, 01_constructor)
{
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("armnn_delegate");
    ASSERT_TRUE(delegate.get() != nullptr);
    DelegateFactory::get().clear();
}

TEST_F(ArmNNDelegateTest, 02_constructor_with_options_cppacc_info)
{
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("armnn_delegate", options_cpuacc_info);
    ASSERT_TRUE(delegate.get() != nullptr);

    std::shared_ptr<ArmNNDelegate> armnnDelegate =
        std::dynamic_pointer_cast<ArmNNDelegate>(delegate);
    auto option = armnnDelegate->getDelegateOptions();
    EXPECT_EQ(option.GetLoggingSeverity(), armnn::LogSeverity::Info);

    auto backends = option.GetBackends();
    EXPECT_EQ(backends.size(), 1);
    EXPECT_EQ(backends[0].Get(), "CpuAcc");
    DelegateFactory::get().clear();
}

TEST_F(ArmNNDelegateTest, 03_constructor_with_options_cppref_debug)
{
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("armnn_delegate", options_cpuref_debug);
    ASSERT_TRUE(delegate.get() != nullptr);

    std::shared_ptr<ArmNNDelegate> armnnDelegate =
        std::dynamic_pointer_cast<ArmNNDelegate>(delegate);
    auto option = armnnDelegate->getDelegateOptions();
    EXPECT_EQ(option.GetLoggingSeverity(), armnn::LogSeverity::Debug);

    auto backends = option.GetBackends();
    EXPECT_EQ(backends.size(), 1);
    EXPECT_EQ(backends[0].Get(), "CpuRef");
    DelegateFactory::get().clear();
}

TEST_F(ArmNNDelegateTest, 04_getTfLiteDelegate)
{
    std::shared_ptr<Delegate> delegate =
        DelegateFactory::get().getDelegate("armnn_delegate", options_cpuacc_info);
    ASSERT_TRUE(delegate.get() != nullptr);
    auto ptr = delegate->getTfLiteDelegate();
    ASSERT_TRUE(ptr.get() != nullptr);
    DelegateFactory::get().clear();
}
