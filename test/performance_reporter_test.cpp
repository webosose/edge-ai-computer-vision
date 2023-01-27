/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>
#include <aif/tools/PerformanceReporter.h>

#include <gtest/gtest.h>
#include <chrono>
#include <thread>

using namespace std;
using namespace aif;

class PerformanceReporterTest: public ::testing::Test
{
protected:
    PerformanceReporterTest() = default;
    ~PerformanceReporterTest() = default;

    void SetUp() override
    {
        AIVision::init();
    }

    void TearDown() override
    {
        AIVision::deinit();
    }

    void sleep(unsigned ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
};

TEST_F(PerformanceReporterTest, str_to_report_type)
{

    PerformanceRecorder r("test model", "test param");
    EXPECT_EQ(PerformanceReporter::strToReportType("NONE"), Performance::NONE);
    EXPECT_EQ(PerformanceReporter::strToReportType("CONSOLE"), Performance::CONSOLE);
    EXPECT_EQ(PerformanceReporter::strToReportType("REPORT"), Performance::REPORT);
}

TEST_F(PerformanceReporterTest, record_type_to_str)
{
    PerformanceRecorder r("test model", "test param");
    EXPECT_TRUE(r.recordTypeToStr(Performance::CREATE_DETECTOR).find("CREATE_DETECTOR") != std::string::npos);
    EXPECT_TRUE(r.recordTypeToStr(Performance::PREPROCESS).find("PREPROCESS") != std::string::npos);
    EXPECT_TRUE(r.recordTypeToStr(Performance::PROCESS).find("PROCESS") != std::string::npos);
    EXPECT_TRUE(r.recordTypeToStr(Performance::POSTPROCESS).find("POSTPROCESS") != std::string::npos);
}

TEST_F(PerformanceReporterTest, add_report_type)
{
    auto& pr = PerformanceReporter::get();

    pr.clear();
    EXPECT_EQ(pr.getReportType(), Performance::NONE);

    pr.addReportType(Performance::CONSOLE);
    EXPECT_EQ(pr.getReportType(), Performance::NONE | Performance::CONSOLE);

    pr.addReportType(Performance::REPORT);
    EXPECT_EQ(pr.getReportType(), Performance::NONE | Performance::CONSOLE | Performance::REPORT);

    pr.clear();
    EXPECT_EQ(pr.getReportType(), Performance::NONE);

    pr.addReportType(Performance::CONSOLE | Performance::REPORT);
    EXPECT_EQ(pr.getReportType(), Performance::CONSOLE | Performance::REPORT);
}

TEST_F(PerformanceReporterTest, recorder_first_infernece)
{
    PerformanceReporter::get().clear();
    PerformanceReporter::get().addReportType(Performance::CONSOLE | Performance::REPORT);
    PerformanceRecorder r("test model", "test param");

    r.start(Performance::CREATE_DETECTOR);
    sleep(10);
    r.stop(Performance::CREATE_DETECTOR);

    r.start(Performance::PREPROCESS);
    sleep(20);
    r.stop(Performance::PREPROCESS);

    r.start(Performance::PROCESS);
    sleep(30);
    r.stop(Performance::PROCESS);

    r.start(Performance::POSTPROCESS);
    sleep(40);
    r.stop(Performance::POSTPROCESS);

    EXPECT_EQ(1, r.getNum(Performance::CREATE_DETECTOR));
    EXPECT_EQ(1, r.getNum(Performance::PREPROCESS));
    EXPECT_EQ(1, r.getNum(Performance::PROCESS));
    EXPECT_EQ(1, r.getNum(Performance::POSTPROCESS));

    r.printAll(true);
}

TEST_F(PerformanceReporterTest, recorder_multiple_inference)
{
    PerformanceReporter::get().clear();
    PerformanceReporter::get().addReportType(Performance::CONSOLE | Performance::REPORT);
    PerformanceRecorder r("test model", "test param");

    r.start(Performance::CREATE_DETECTOR);
    sleep(10);
    r.stop(Performance::CREATE_DETECTOR);

    for (int i = 0; i < 10; i++) {
        r.start(Performance::PREPROCESS);
        sleep(20);
        r.stop(Performance::PREPROCESS);

        r.start(Performance::PROCESS);
        sleep(30);
        r.stop(Performance::PROCESS);

        r.start(Performance::POSTPROCESS);
        sleep(40);
        r.stop(Performance::POSTPROCESS);
    }

    EXPECT_EQ(1, r.getNum(Performance::CREATE_DETECTOR));
    EXPECT_EQ(10, r.getNum(Performance::PREPROCESS));
    EXPECT_EQ(10, r.getNum(Performance::PROCESS));
    EXPECT_EQ(10, r.getNum(Performance::POSTPROCESS));

    r.printAll(true);
}

TEST_F(PerformanceReporterTest, recorder_type_console_test)
{
    PerformanceReporter::get().clear();
    PerformanceReporter::get().addReportType(Performance::CONSOLE);
    PerformanceRecorder r("test model", "test param");

    r.start(Performance::CREATE_DETECTOR);
    sleep(10);
    r.stop(Performance::CREATE_DETECTOR);

    for (int i = 0; i < 10; i++) {
        r.start(Performance::PREPROCESS);
        sleep(20);
        r.stop(Performance::PREPROCESS);

        r.start(Performance::PROCESS);
        sleep(30);
        r.stop(Performance::PROCESS);

        r.start(Performance::POSTPROCESS);
        sleep(40);
        r.stop(Performance::POSTPROCESS);
    }

    EXPECT_EQ(0, r.getNum(Performance::CREATE_DETECTOR));
    EXPECT_EQ(0, r.getNum(Performance::PREPROCESS));
    EXPECT_EQ(0, r.getNum(Performance::PROCESS));
    EXPECT_EQ(0, r.getNum(Performance::POSTPROCESS));
    r.printAll(true);
}

TEST_F(PerformanceReporterTest, recorder_report_type_report_test)
{
    PerformanceReporter::get().clear();
    PerformanceReporter::get().addReportType(Performance::REPORT);
    PerformanceRecorder r("test model", "test param");

    r.start(Performance::CREATE_DETECTOR);
    sleep(10);
    r.stop(Performance::CREATE_DETECTOR);

    for (int i = 0; i < 10; i++) {
        r.start(Performance::PREPROCESS);
        sleep(20);
        r.stop(Performance::PREPROCESS);

        r.start(Performance::PROCESS);
        sleep(30);
        r.stop(Performance::PROCESS);

        r.start(Performance::POSTPROCESS);
        sleep(40);
        r.stop(Performance::POSTPROCESS);
    }

    EXPECT_EQ(1, r.getNum(Performance::CREATE_DETECTOR));
    EXPECT_EQ(10, r.getNum(Performance::PREPROCESS));
    EXPECT_EQ(10, r.getNum(Performance::PROCESS));
    EXPECT_EQ(10, r.getNum(Performance::POSTPROCESS));
    r.printAll(true);
}

TEST_F(PerformanceReporterTest, recorder_report_type_none_test)
{
    PerformanceReporter::get().clear();
    PerformanceReporter::get().addReportType(Performance::NONE);
    PerformanceRecorder r("test model", "test param");

    r.start(Performance::CREATE_DETECTOR);
    sleep(10);
    r.stop(Performance::CREATE_DETECTOR);

    for (int i = 0; i < 10; i++) {
        r.start(Performance::PREPROCESS);
        sleep(20);
        r.stop(Performance::PREPROCESS);

        r.start(Performance::PROCESS);
        sleep(30);
        r.stop(Performance::PROCESS);

        r.start(Performance::POSTPROCESS);
        sleep(40);
        r.stop(Performance::POSTPROCESS);
    }

    EXPECT_EQ(0, r.getNum(Performance::CREATE_DETECTOR));
    EXPECT_EQ(0, r.getNum(Performance::PREPROCESS));
    EXPECT_EQ(0, r.getNum(Performance::PROCESS));
    EXPECT_EQ(0, r.getNum(Performance::POSTPROCESS));
    r.printAll(true);
}

TEST_F(PerformanceReporterTest, reporter_test)
{
    PerformanceReporter::get().clear();
    int type = Performance::CONSOLE | Performance::REPORT;
    PerformanceReporter::get().addReportType(type);
    EXPECT_EQ(type, PerformanceReporter::get().getReportType());

    auto r1 = std::make_shared<PerformanceRecorder>("test model1", "test param1");
    auto r2 = std::make_shared<PerformanceRecorder>("test model2", "test param2");
    PerformanceReporter::get().addRecorder(r1);
    PerformanceReporter::get().addRecorder(r2);

    for (int i = 0; i < 10; i++) {
        r1->start(Performance::CREATE_DETECTOR);
        sleep(10);
        r1->stop(Performance::CREATE_DETECTOR);

        r1->start(Performance::PREPROCESS);
        sleep(20);
        r1->stop(Performance::PREPROCESS);

        r1->start(Performance::PROCESS);
        sleep(30);
        r1->stop(Performance::PROCESS);

        r1->start(Performance::POSTPROCESS);
        sleep(40);
        r1->stop(Performance::POSTPROCESS);

        r2->start(Performance::CREATE_DETECTOR);
        sleep(10);
        r2->stop(Performance::CREATE_DETECTOR);

        r2->start(Performance::PREPROCESS);
        sleep(20);
        r2->stop(Performance::PREPROCESS);

        r2->start(Performance::PROCESS);
        sleep(30);
        r2->stop(Performance::PROCESS);

        r2->start(Performance::POSTPROCESS);
        sleep(40);
        r2->stop(Performance::POSTPROCESS);
    }
    EXPECT_EQ(10, r1->getNum(Performance::CREATE_DETECTOR));
    EXPECT_EQ(10, r1->getNum(Performance::PREPROCESS));
    EXPECT_EQ(10, r1->getNum(Performance::PROCESS));
    EXPECT_EQ(10, r1->getNum(Performance::POSTPROCESS));

    EXPECT_EQ(10, r2->getNum(Performance::CREATE_DETECTOR));
    EXPECT_EQ(10, r2->getNum(Performance::PREPROCESS));
    EXPECT_EQ(10, r2->getNum(Performance::PROCESS));
    EXPECT_EQ(10, r2->getNum(Performance::POSTPROCESS));
    PerformanceReporter::get().showReport();
}

TEST_F(PerformanceReporterTest, reporter_with_detector_test)
{
    PerformanceReporter::get().clear();
    PerformanceReporter::get().addReportType(Performance::CONSOLE | Performance::REPORT);

    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::FACE;
    EdgeAIVision::DetectorType type2 = EdgeAIVision::DetectorType::POSE;

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.createDetector(type2));
    for (int i = 0; i < 10; i++) {
        EXPECT_TRUE(ai.detect(type, input, output));
        EXPECT_TRUE(ai.detect(type2, input, output));
    }
    EXPECT_TRUE(ai.deleteDetector(type));
    EXPECT_TRUE(ai.deleteDetector(type2));
    ai.shutdown();

    PerformanceReporter::get().showReport();
}
