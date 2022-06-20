/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/Delegate.h>
#include <aif/base/DelegateFactory.h>
#include <aif/base/DelegateFactoryRegistrations.h>
#include <aif/base/AIVision.h>

#include <gtest/gtest.h>
#include <iostream>

using namespace aif;

class TestDelegate1 : public Delegate
{
public:
    TestDelegate1( const std::string& name, const std::string& option = "")
        : Delegate(name, option) {}

    TfLiteDelegatePtr getTfLiteDelegate() const override {
        return TfLiteDelegatePtr(nullptr, [](TfLiteDelegate* d) {});
    }
};
DelegateFactoryRegistration<TestDelegate1> test_delegate1("test_delegate1");

class TestDelegate2 : public Delegate
{
public:
    TestDelegate2( const std::string& name, const std::string& option = "")
        : Delegate(name, option) {}

    TfLiteDelegatePtr getTfLiteDelegate() const override {
        return TfLiteDelegatePtr(nullptr, [](TfLiteDelegate* d) {});
    }
};
DelegateFactoryRegistration<TestDelegate2> test_delegate2("test_delegate2");


class DelegateFactoryTest : public ::testing::Test
{
protected:
    DelegateFactoryTest() = default;
    ~DelegateFactoryTest() = default;

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
};

TEST_F(DelegateFactoryTest, 01_getDelegate)
{
    DelegateFactory* factory = &DelegateFactory::get();
    EXPECT_TRUE(factory != nullptr);
    EXPECT_EQ(factory->getNumDelegates(), 0);
    EXPECT_TRUE(factory->getDelegate("test_delegate") == nullptr);
    EXPECT_EQ(factory->getNumDelegates(), 0);
    EXPECT_TRUE(factory->getDelegate("test_delegate1") != nullptr);
    EXPECT_EQ(factory->getNumDelegates(), 1);
    EXPECT_TRUE(factory->getDelegate("test_delegate") == nullptr);
    EXPECT_EQ(factory->getNumDelegates(), 1);
    EXPECT_TRUE(factory->getDelegate("test_delegate2") != nullptr);
    EXPECT_EQ(factory->getNumDelegates(), 2);
}

TEST_F(DelegateFactoryTest, 02_clear)
{
    DelegateFactory* factory = &DelegateFactory::get();
    EXPECT_TRUE(factory != nullptr);
    EXPECT_TRUE(factory->getDelegate("test_delegate1") != nullptr);
    EXPECT_TRUE(factory->getDelegate("test_delegate2") != nullptr);
    EXPECT_EQ(factory->getNumDelegates(), 2);
    factory->clear();
    EXPECT_EQ(factory->getNumDelegates(), 0);
}
