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


TEST_F(DescriptorTest, descriptor_addResponseName_test)
{
    Descriptor ds;
    ds.addResponseName("response", "testid");

    EXPECT_EQ(ds.toStr(),
            "{\"response\":\"response\",\"id\":\"testid\"}");
}

TEST_F(DescriptorTest, descriptor_addReturnCode_test)
{
    Descriptor ds;
    ds.addResponseName("response", "testid");
    ds.addReturnCode(104);

    EXPECT_EQ(ds.toStr(),
            "{\"response\":\"response\",\"id\":\"testid\",\"returnCode\":104}");
}
