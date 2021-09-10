#include <aif/face/FaceParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace aif;

class FaceParamTest : public ::testing::Test
{
protected:
    FaceParamTest() = default;
    ~FaceParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

/*
FaceParam(
        const std::vector<int>& strides = {8, 16, 16, 16},
        const std::vector<float>& optAspectRatios = {1.0f},
        float interpolatedScaleAspectRatio = 1.0f,
        float anchorOffsetX = 0.5,
        float anchorOffsetY = 0.5,
        float minScale = 0.1484375,
        float maxScale = 0.75,
        bool  reduceBoxesInLowestLayer = false,
        float scoreThreshold = 0.7f,
        float iouThreshold = 0.2f,
        int   maxOutputSize = 100
    );
*/
TEST_F(FaceParamTest, faceparam01_constructor_test)
{
    // default constructor
    FaceParam fp;
    fp.trace();

    FaceParam fp2
    {
        {8, 16, 16, 16},
        {1.0f},
        1.0f,
        0.5,
        0.5,
        0.1484375,
        0.75,
        false,
        0.7f,
        0.2f,
        100
    };
    fp2.trace();

    EXPECT_EQ(fp, fp2);
}

TEST_F(FaceParamTest, faseparam02_copy_constructors)
{
    // constructor 1
    FaceParam fp1;
    fp1.trace();

    // copy constructor
    FaceParam fp2(fp1);
    EXPECT_EQ(fp1, fp2);

    // copy assignment operation
    FaceParam fp3;
    fp3 = fp1;
    ASSERT_EQ(fp3, fp1);
}

TEST_F(FaceParamTest, faceparam03_move_constructors)
{
    // constructor 1
    FaceParam fp1;
    fp1.trace();

    // copy constructor
    FaceParam fp2(fp1);
    FaceParam fp3(fp1);

    // move constructor
    FaceParam fp4(std::move(fp2));

    EXPECT_EQ(fp4, fp1);

    // move assignment operation
    FaceParam fp5;
    fp5 = std::move(fp3);

    EXPECT_EQ(fp5, fp1);
}
