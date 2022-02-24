#include <aif/base/DetectorFactory.h>
#include <aif/base/DelegateFactory.h>
#include <aif/poseLandmark/PoseLandmarkDescriptor.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class PoseLandmarkDetectorTest : public ::testing::Test
{
protected:
    PoseLandmarkDetectorTest() = default;
    ~PoseLandmarkDetectorTest() = default;

    void SetUp() override
    {
        DetectorFactory::get().clear();
    }

    void TearDown() override
    {
    }
};

TEST_F(PoseLandmarkDetectorTest, 01_cpu_init)
{
    auto pd = DetectorFactory::get().getDetector("poselandmark_lite_cpu");
    EXPECT_TRUE(pd.get() != nullptr);
    EXPECT_EQ(pd->getModelPath(), "/usr/share/aif/model/pose_landmark_lite.tflite");
    auto modelInfo = pd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 256);
    EXPECT_EQ(modelInfo.width, 256);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(PoseLandmarkDetectorTest, 02_cpu_from_person)
{
    auto pd = DetectorFactory::get().getDetector("poselandmark_lite_cpu");
    EXPECT_TRUE(pd.get() != nullptr);
    
    PoseLandmarkDescriptor* poselandmarkDescriptor = new PoseLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(poselandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("poses"));
    EXPECT_TRUE(pd->detectFromImage("/usr/share/aif/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("poses"));
}


TEST_F(PoseLandmarkDetectorTest, 03_cpu_from_people)
{
    auto pd = DetectorFactory::get().getDetector("poselandmark_lite_cpu");
    EXPECT_TRUE(pd.get() != nullptr);
    
    PoseLandmarkDescriptor* poselandmarkDescriptor = new PoseLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(poselandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("poses"));
    EXPECT_TRUE(pd->detectFromImage("/usr/share/aif/images/people.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("poses"));
}

TEST_F(PoseLandmarkDetectorTest, 04_cpu_poselandmark_from_base64_mona)
{
    auto pd = DetectorFactory::get().getDetector("poselandmark_lite_cpu");
    EXPECT_TRUE(pd.get() != nullptr);

    auto base64image = aif::fileToStr("/usr/share/aif/images/mona_base64.txt"); // 128 x 128
    PoseLandmarkDescriptor* poselandmarkDescriptor = new PoseLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(poselandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("poses"));
    EXPECT_TRUE(pd->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("poses"));
}
