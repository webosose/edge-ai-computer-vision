#include <aif/base/Descriptor.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>

#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>

using namespace aif;

class DescriptorTest : public ::testing::Test
{
protected:
    DescriptorTest() = default;
    ~DescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};


TEST_F(DescriptorTest, 01_addResponseName)
{
    Descriptor ds;
    ds.addResponseName("response", "testid");

    EXPECT_EQ(ds.toStr(),
            "{\"response\":\"response\",\"id\":\"testid\"}");
}

TEST_F(DescriptorTest, 02_addReturnCode)
{
    Descriptor ds;
    ds.addResponseName("response", "testid");
    ds.addReturnCode(104);

    EXPECT_EQ(ds.toStr(),
            "{\"response\":\"response\",\"id\":\"testid\",\"returnCode\":104}");
}

TEST_F(DescriptorTest, 03_hasMember)
{
    Descriptor ds;
    ds.addResponseName("response", "testid");
    ds.addReturnCode(104);

    EXPECT_TRUE(ds.hasMember("response"));
    EXPECT_TRUE(ds.hasMember("id"));
    EXPECT_TRUE(ds.hasMember("returnCode"));
}
