#include <aif/base/DetectorFactory.h>
#include <aif/base/DelegateFactory.h>
#include <aif/movenet/MovenetDescriptor.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class MovenetDetectorTest : public ::testing::Test
{
protected:
    MovenetDetectorTest() = default;
    ~MovenetDetectorTest() = default;

    void SetUp() override
    {
        DetectorFactory::get().clear();
    }

    void TearDown() override
    {
    }

    std::string armnn_delegate_param {
        "{"
        "  \"common\" : {"
        "      \"useXnnpack\": true,"
        "      \"numThreads\": 1"
        "    },"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"armnn_delegate\","
        "      \"option\": {"
        "        \"backends\": [\"CpuAcc\"],"
        "        \"logging_severity\": \"info\""
        "      }"
        "    }"
        "  ]"
        "}"
    };


};


TEST_F(MovenetDetectorTest, 01_cpu_init)
{
    auto pd = DetectorFactory::get().getDetector("movenet_thunder_cpu");
    EXPECT_TRUE(pd.get() != nullptr);
    EXPECT_EQ(pd->getModelPath(), "/usr/share/aif/model/movenet_single_pose_thunder_ptq.tflite");
    auto modelInfo = pd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 256);
    EXPECT_EQ(modelInfo.width, 256);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(MovenetDetectorTest, 02_cpu_from_person)
{
    auto pd = DetectorFactory::get().getDetector("movenet_thunder_cpu");
    EXPECT_TRUE(pd.get() != nullptr);
    
    MovenetDescriptor* movenetDescriptor = new MovenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(movenetDescriptor);
    EXPECT_TRUE(pd->detectFromImage("/usr/share/aif/images/person.jpg", descriptor) == aif::kAifOk);
    //EXPECT_EQ(movenetDescriptor->getPoseCount(), 1);
}


TEST_F(MovenetDetectorTest, 03_cpu_from_people)
{
    auto pd = DetectorFactory::get().getDetector("movenet_thunder_cpu");
    EXPECT_TRUE(pd.get() != nullptr);
    
    MovenetDescriptor* movenetDescriptor = new MovenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(movenetDescriptor);
    EXPECT_FALSE(descriptor->hasMember("poses"));
    EXPECT_TRUE(pd->detectFromImage("/usr/share/aif/images/people.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("poses"));
}

TEST_F(MovenetDetectorTest, 04_cpu_movenet_from_base64_mona)
{
    auto pd = DetectorFactory::get().getDetector("movenet_thunder_cpu");
    EXPECT_TRUE(pd.get() != nullptr);

    auto base64image = aif::fileToStr("/usr/share/aif/images/mona_base64.txt"); // 128 x 128
    MovenetDescriptor* movenetDescriptor = new MovenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(movenetDescriptor);
    EXPECT_FALSE(descriptor->hasMember("poses"));
    EXPECT_TRUE(pd->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("poses"));
}

#ifdef USE_EDGETPU
TEST_F(MovenetDetectorTest, 05_edgetpu_from_person)
{
    auto pd = DetectorFactory::get().getDetector("movenet_thunder_edgetpu");
    EXPECT_TRUE(pd.get() != nullptr);

    MovenetDescriptor* movenetDescriptor = new MovenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(movenetDescriptor);
    EXPECT_FALSE(descriptor->hasMember("poses"));
    EXPECT_TRUE(pd->detectFromImage("/usr/share/aif/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("poses"));
}
#endif
