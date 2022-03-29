#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>

using namespace std;
using namespace aif;

class AIVisionTest: public ::testing::Test
{
protected:
    AIVisionTest() = default;
    ~AIVisionTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(AIVisionTest, 01_intialized)
{
    EXPECT_FALSE(AIVision::isInitialized());
    AIVision::init();
    EXPECT_TRUE(AIVision::isInitialized());
    AIVision::deinit();
    EXPECT_FALSE(AIVision::isInitialized());
}

TEST_F(AIVisionTest, 02_getBasePath)
{
    AIVision::deinit();
    AIVision::init();
    EXPECT_TRUE(AIVision::isInitialized());
    EXPECT_EQ(AIVision::getBasePath(), EDGEAI_VISION_HOME);
    EXPECT_EQ(AIVision::getModelFolderPath(),
            string(EDGEAI_VISION_HOME) +  "/model/");
    AIVision::deinit();
}
