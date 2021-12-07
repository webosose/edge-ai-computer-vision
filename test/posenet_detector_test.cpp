#include <aif/base/DetectorFactory.h>
#include <aif/base/DelegateFactory.h>
#include <aif/pose/PosenetDescriptor.h>
//#ifdef USE_EDGETPU
//#include <aif/pose/EdgeTpuPosenetDetector.h>
//#endif
//#ifdef USE_ARMNN
//#include <aif/pose/ArmNNPosenetDetector.h>
//#endif

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
        DetectorFactory::get().clear();
    }

    void TearDown() override
    {
    }
};


TEST_F(PosenetDetectorTest, pd01_cpu_init)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu");
    EXPECT_TRUE(pd.get() != nullptr);
    EXPECT_EQ(pd->getModelPath(), "/usr/share/aif/model/posenet_mobilenet_v1_075_353_481_quant_decoder.tflite");
    auto modelInfo = pd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 353);
    EXPECT_EQ(modelInfo.width, 481);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(PosenetDetectorTest, pd02_cpu_from_person)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu");
    EXPECT_TRUE(pd.get() != nullptr);
    
    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_TRUE(pd->detectFromImage("/usr/share/aif/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}


TEST_F(PosenetDetectorTest, pd03_cpu_from_people)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu");
    EXPECT_TRUE(pd.get() != nullptr);
    
    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_TRUE(pd->detectFromImage("/usr/share/aif/images/people.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 5);
}

TEST_F(PosenetDetectorTest, pd04_cpu_posenet_from_base64_person)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu");
    EXPECT_TRUE(pd.get() != nullptr);

    auto base64image = aif::fileToStr("/usr/share/aif/images/mona_base64.txt"); // 128 x 128
    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_TRUE(pd->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}

#ifdef USE_EDGETPU
TEST_F(PosenetDetectorTest, pd05_edgetpu_from_person)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_edgetpu");
    EXPECT_TRUE(pd.get() != nullptr);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_TRUE(pd->detectFromImage("/usr/share/aif/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}
#endif

#ifdef USE_ARMNN
std::string param(
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
);

TEST_F(PosenetDetectorTest, pd06_armnn_posenet_from_person_default)
{
    std::cout << param << std::endl;
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu", param);
    EXPECT_TRUE(pd.get() != nullptr);
    EXPECT_TRUE(pd->getNumDelegates() == 1);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_TRUE(pd->detectFromImage("/usr/share/aif/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}

TEST_F(PosenetDetectorTest, pd07_armnn_posenet_from_person_cpuAcc)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu", param);
    EXPECT_TRUE(pd.get() != nullptr);
    EXPECT_TRUE(pd->getNumDelegates() == 1);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_TRUE(pd->detectFromImage("/usr/share/aif/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}
#endif
