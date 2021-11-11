//#include <aif/pose/PosenetDetectorFactory.h>
#include <aif/pose/CpuPosenetDetector.h>
#ifdef USE_EDGETPU
#include <aif/pose/EdgeTpuPosenetDetector.h>
#endif

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class PosenetDetectorTest : public ::testing::Test
{
protected:
    PosenetDetectorTest() = default;
    ~PosenetDetectorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};


TEST_F(PosenetDetectorTest, pd01_cpu_init)
{
    CpuPosenetDetector pd;
    EXPECT_EQ(pd.init(), kAifOk);
    EXPECT_EQ(pd.getModelPath(), "/usr/share/aif/model/posenet_mobilenet_v1_075_353_481_quant_decoder.tflite");
    auto modelInfo = pd.getModelInfo();
    EXPECT_EQ(modelInfo.height, 353);
    EXPECT_EQ(modelInfo.width, 481);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(PosenetDetectorTest, pd02_cpu_from_person)
{
    CpuPosenetDetector pd;
    EXPECT_EQ(pd.init(), kAifOk);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_TRUE(pd.detectFromImage("/usr/share/aif/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}


TEST_F(PosenetDetectorTest, pd03_cpu_from_people)
{
    CpuPosenetDetector pd;
    EXPECT_EQ(pd.init(), kAifOk);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_TRUE(pd.detectFromImage("/usr/share/aif/images/people.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 5);
}

TEST_F(PosenetDetectorTest, pd04_cpu_posenet_from_base64_person)
{
    CpuPosenetDetector pd;
    EXPECT_EQ(pd.init(), kAifOk);

    auto base64image = aif::fileToStr("/usr/share/aif/images/mona_base64.txt"); // 128 x 128
    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_TRUE(pd.detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}

#ifdef USE_EDGETPU
TEST_F(PosenetDetectorTest, pd05_edgetpu_from_person)
{
    EdgeTpuPosenetDetector pd;
    EXPECT_EQ(pd.init(), kAifOk);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_TRUE(pd.detectFromImage("/usr/share/aif/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}
#endif
