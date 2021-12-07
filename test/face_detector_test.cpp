#include <aif/base/DetectorFactory.h>
#include <aif/base/Detector.h>
#include <aif/base/Descriptor.h>
#include <aif/face/FaceDescriptor.h>
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
        DetectorFactory::get().clear();
    }

    void TearDown() override
    {
    }
};

TEST_F(FaceDetectorTest, fd01_short_range_init)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelPath(), "/usr/share/aif/model/face_detection_short_range.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 128);
    EXPECT_EQ(modelInfo.width, 128);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(FaceDetectorTest, fd02_short_range_detect_faces_from_mona)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage("/usr/share/aif/images/mona.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(FaceDetectorTest, fd03_short_range_detect_faces_from_bts)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage("/usr/share/aif/images/bts.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 7);
}

TEST_F(FaceDetectorTest, fd04_short_range_detect_faces_from_blackpink)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage("/usr/share/aif/images/blackpink.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 4);
}

#if 0
TEST_F(FaceDetectorTest, fd05_short_range_detect_faces_from_doctors)
{
    ShortRangeFaceDetector fd;
    auto faceParam = fd->getFaceParam();
    faceParam.interpolatedScaleAspectRatio = 473/670;
    faceParam.scoreThreshold = 0.45f;
    fd->setFaceParam(faceParam);
    EXPECT_EQ(fd->init(), kAifOk);
    auto foundFaces = std::make_shared<FaceDescriptor>();
    EXPECT_TRUE(fd->detectFromImage("/usr/share/aif/images/doctors.jpg", foundFaces) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 5);
}
#endif

TEST_F(FaceDetectorTest, fd06_full_range_load_model)
{
    auto fd = DetectorFactory::get().getDetector("face_full_range_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelPath(), "/usr/share/aif/model/face_detection_full_range.tflite");

    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 192);
    EXPECT_EQ(modelInfo.width, 192);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(FaceDetectorTest, fd07_full_range_detect_faces_from_mona)
{
    auto fd = DetectorFactory::get().getDetector("face_full_range_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage("/usr/share/aif/images/mona.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(FaceDetectorTest, fd08_full_range_detect_faces_from_bts)
{
    auto fd = DetectorFactory::get().getDetector("face_full_range_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage("/usr/share/aif/images/bts.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 7);
}

TEST_F(FaceDetectorTest, fd09_full_range_detect_faces_from_blackpink)
{
    auto fd = DetectorFactory::get().getDetector("face_full_range_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage("/usr/share/aif/images/blackpink.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 4);
}

TEST_F(FaceDetectorTest, fd11_short_range_detect_faces_from_base64mona)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_cpu");
    EXPECT_TRUE(fd.get() != nullptr);

    auto base64image = aif::fileToStr("/usr/share/aif/images/mona_base64.txt"); // 128 x 128
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

#ifdef USE_EDGETPU
TEST_F(FaceDetectorTest, edgetpu01_short_range_detect_faces_from_base64mona)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_edgetpu");
    EXPECT_TRUE(fd.get() != nullptr);

    auto base64image = aif::fileToStr("/usr/share/aif/images/mona_base64.txt"); // 128 x 128
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}
#endif
