#include <aif/face/FaceDetectorFactory.h>
#include <aif/face/CpuFaceDetector.h>
#ifdef USE_EDGETPU
#include <aif/face/EdgeTpuFaceDetector.h>
#endif

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class FaceDetectorTest : public ::testing::Test
{
protected:
    FaceDetectorTest() = default;
    ~FaceDetectorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(FaceDetectorTest, fd01_short_range_init)
{
    ShortRangeFaceDetector fd;
    EXPECT_EQ(fd.init(), kAifOk);
    EXPECT_EQ(fd.getModelPath(), "model/face_detection_short_range.tflite");
    auto modelInfo = fd.getModelInfo();
    EXPECT_EQ(modelInfo.height, 128);
    EXPECT_EQ(modelInfo.width, 128);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(FaceDetectorTest, fd02_short_range_detect_faces_from_mona)
{
    ShortRangeFaceDetector fd;
    EXPECT_EQ(fd.init(), kAifOk);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);

    EXPECT_TRUE(fd.detectFromImage("images/mona.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);

    auto fd2 = FaceDetectorFactory::create("short_range");
    EXPECT_EQ(fd2->init(), kAifOk);
    foundFaces->clear();
    EXPECT_TRUE(fd2->detectFromImage("images/mona.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(FaceDetectorTest, fd03_short_range_detect_faces_from_bts)
{
    ShortRangeFaceDetector fd;
    EXPECT_EQ(fd.init(), kAifOk);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);

    EXPECT_TRUE(fd.detectFromImage("images/bts.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 7);

}

TEST_F(FaceDetectorTest, fd04_short_range_detect_faces_from_blackpink)
{
    ShortRangeFaceDetector fd;
    EXPECT_EQ(fd.init(), kAifOk);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd.detectFromImage("images/blackpink.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 4);
}

#if 0
TEST_F(FaceDetectorTest, fd05_short_range_detect_faces_from_doctors)
{
    ShortRangeFaceDetector fd;
    auto faceParam = fd.getFaceParam();
    faceParam.interpolatedScaleAspectRatio = 473/670;
    faceParam.scoreThreshold = 0.45f;
    fd.setFaceParam(faceParam);
    EXPECT_EQ(fd.init(), kAifOk);
    auto foundFaces = std::make_shared<FaceDescriptor>();
    EXPECT_TRUE(fd.detectFromImage("images/doctors.jpg", foundFaces) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 5);
}
#endif

TEST_F(FaceDetectorTest, fd06_full_range_load_model)
{
    FullRangeFaceDetector fd;
    EXPECT_EQ(fd.init(), kAifOk);
    EXPECT_EQ(fd.getModelPath(), "model/face_detection_full_range.tflite");

    auto modelInfo = fd.getModelInfo();
    EXPECT_EQ(modelInfo.height, 192);
    EXPECT_EQ(modelInfo.width, 192);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(FaceDetectorTest, fd07_full_range_detect_faces_from_mona)
{
    FullRangeFaceDetector fd;
    EXPECT_EQ(fd.init(), kAifOk);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd.detectFromImage("images/mona.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);

    auto fd2 = FaceDetectorFactory::create("full_range");
    EXPECT_EQ(fd2->init(), kAifOk);
    foundFaces->clear();
    EXPECT_TRUE(fd2->detectFromImage("images/mona.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(FaceDetectorTest, fd08_full_range_detect_faces_from_bts)
{
    FullRangeFaceDetector fd;
    EXPECT_EQ(fd.init(), kAifOk);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd.detectFromImage("images/bts.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 7);
}

TEST_F(FaceDetectorTest, fd09_full_range_detect_faces_from_blackpink)
{
    FullRangeFaceDetector fd;
    EXPECT_EQ(fd.init(), kAifOk);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd.detectFromImage("images/blackpink.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 4);
}

#if 0
TEST_F(FaceDetectorTest, fd10_full_range_detect_faces_from_doctors)
{
    FullRangeFaceDetector fd;
    auto faceParam = fd.getFaceParam();
    faceParam.interpolatedScaleAspectRatio = 473/670;
    faceParam.scoreThreshold = 0.55f;
    fd.setFaceParam(faceParam);
    EXPECT_EQ(fd.init(), kAifOk);
    detectWithImage(fd, "images/doctors.jpg", 5);
}
#endif

TEST_F(FaceDetectorTest, fd11_short_range_detect_faces_from_base64mona)
{
    ShortRangeFaceDetector fd;
    EXPECT_EQ(fd.init(), kAifOk);

    auto base64image = aif::fileToStr("images/mona_base64.txt"); // 128 x 128
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd.detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);

    auto fd2 = FaceDetectorFactory::create("short_range");
    EXPECT_EQ(fd2->init(), kAifOk);
    foundFaces->clear();
    EXPECT_TRUE(fd2->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

#ifdef USE_EDGETPU
TEST_F(FaceDetectorTest, edgetpu01_short_range_detect_faces_from_base64mona)
{
    EdgeTpuShortRangeFaceDetector fd;
    EXPECT_EQ(fd.init(), kAifOk);

    auto base64image = aif::fileToStr("images/mona_base64.txt"); // 128 x 128
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd.detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);

    auto fd2 = FaceDetectorFactory::create("short_range_edgetpu");
    EXPECT_EQ(fd2->init(), kAifOk);
    foundFaces->clear();
    EXPECT_TRUE(fd2->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}
#endif
