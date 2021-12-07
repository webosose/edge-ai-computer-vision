#include <aif/base/Detector.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>

#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>

using namespace aif;

class TestDetector1 : public Detector
{
public:
    TestDetector1() : Detector("/usr/share/aif/model/face_detection_short_range.tflite") {}

    t_aif_status init(const std::string& param) override {
        return kAifOk;
    }
    std::shared_ptr<DetectorParam> createParam() override {
        return std::make_shared<DetectorParam>();
    }
    t_aif_status fillInputTensor(const cv::Mat& img) override{
        return kAifOk;
    }
    t_aif_status preProcessing() {
        return kAifOk;
    }
    t_aif_status postProcessing(
            const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override {
        return kAifOk;
    }
    t_aif_status compileModel() override {
        return kAifOk;
    }
};
DetectorFactoryRegistration<TestDetector1, Descriptor>
test_detector1("test_detector1");

class TestDetector2 : public TestDetector1
{
public:
    TestDetector2() {}
};
DetectorFactoryRegistration<TestDetector2, Descriptor>
test_Detector2("test_detector2");


class DetectorFactoryTest : public ::testing::Test
{
protected:
    DetectorFactoryTest() = default;
    ~DetectorFactoryTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};


TEST_F(DetectorFactoryTest, detector_factory_getDetector_test)
{
    DetectorFactory* factory = &DetectorFactory::get();
    EXPECT_TRUE(factory != nullptr);
    EXPECT_EQ(factory->getNumDetectors(), 0);
    EXPECT_TRUE(factory->getDetector("test_detector1") != nullptr);
    EXPECT_EQ(factory->getNumDetectors(), 1);
    EXPECT_TRUE(factory->getDetector("test_detector2") != nullptr);
    EXPECT_EQ(factory->getNumDetectors(), 2);
}

TEST_F(DetectorFactoryTest, detector_factory_clear_test)
{
    DetectorFactory* factory = &DetectorFactory::get();
    EXPECT_TRUE(factory != nullptr);
    EXPECT_TRUE(factory->getDetector("test_detector1") != nullptr);
    EXPECT_TRUE(factory->getDetector("test_detector2") != nullptr);
    EXPECT_EQ(factory->getNumDetectors(), 2);
    factory->clear();
    EXPECT_EQ(factory->getNumDetectors(), 0);
}
