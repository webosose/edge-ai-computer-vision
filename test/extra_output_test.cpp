/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/Detector.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/pipe/Pipe.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

using namespace aif;

class ExtraOutputTestData {
    public:
        std::vector<float> data_float = { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f };
        std::vector<uint8_t> data_uint8 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        std::vector<int8_t> data_int8  = { -5, -4, -3, -2, -1, 0, 1, 2, 3, 4};
};

class ExtraOutputTestDetector : public Detector {
  public:
    ExtraOutputTestDetector() : Detector("face_detection_short_range.tflite") {
        std::cout << "create test detctor" << std::endl;
    }

    std::shared_ptr<DetectorParam> createParam() override {
        std::cout << "create param" << std::endl;
        return std::make_shared<DetectorParam>();
    }
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override {
        tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter);
        return kAifOk;
    }
    t_aif_status fillInputTensor(const cv::Mat &img) override {
        return kAifOk;
    }
    t_aif_status preProcessing() {
        return kAifOk;
    }
    t_aif_status
    postProcessing(const cv::Mat &img,
                   std::shared_ptr<Descriptor> &descriptor) override {

        const auto& extraOutput = descriptor->getExtraOutput();
        std::cout << "type: " << static_cast<int>(extraOutput.type()) << std::endl;
        std::cout << "bytes : " << extraOutput.bytes() << std::endl;
        std::cout << "buffer : " << extraOutput.buffer() << std::endl;

        if (extraOutput.type() == ExtraOutputType::FLOAT_ARRAY) {
            // direct memcpy
            memcpy(extraOutput.buffer(),
                   d.data_float.data(),
                   sizeof(float) * d.data_float.size());
        } else if (extraOutput.type() == ExtraOutputType::UINT8_ARRAY) {
            // using descriptor addExtraOutput interface
            descriptor->addExtraOutput(
                    ExtraOutputType::UINT8_ARRAY,
                    static_cast<void*>(d.data_uint8.data()),
                    sizeof(uint8_t) * d.data_uint8.size());
        } else if (extraOutput.type() == ExtraOutputType::INT8_ARRAY) {
            // using descriptor addExtraOutput interface
            descriptor->addExtraOutput(
                    ExtraOutputType::INT8_ARRAY,
                    static_cast<void*>(d.data_int8.data()),
                    sizeof(uint8_t) * d.data_int8.size());
        }

        return kAifOk;
    }
    ExtraOutputTestData d;
};
DetectorFactoryRegistration<ExtraOutputTestDetector, Descriptor>
    extra_output_test_detector1("extra_output_test_detector");


class ExtraOutputTest : public ::testing::Test {
  protected:
    ExtraOutputTest() = default;
    ~ExtraOutputTest() = default;

    static void SetUpTestCase() { AIVision::init(); }

    static void TearDownTestCase() { AIVision::deinit(); }

    void SetUp() override { basePath = AIVision::getBasePath(); }

    void TearDown() override {}

    std::string basePath;
    std::string option { R"(
       {
           "model": "extra_output_test_detector",
           "param": {}
       })"
    };
    std::string pipeConfig = R"(
        {
            "name" : "pipe_face",
            "nodes": [
                {
                    "id" : "detect_face",
                    "input" : ["image"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "detector",
                        "config": {
                            "model": "extra_output_test_detector",
                            "param": {}
                        }
                    }
                }
            ]
        }
    )";
    std::string pipeId = "pipe_face";
    std::string nodeId = "detect_face";

    ExtraOutputTestData d;


    void testPipe(const ExtraOutputs& extraOutputs) {
        cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);

        AIVision::init();
        Pipe pipe;
        EXPECT_TRUE(pipe.build(pipeConfig));
        EXPECT_TRUE(pipe.detect(input, extraOutputs));

        auto descriptor = pipe.getDescriptor();
        EXPECT_TRUE(descriptor != nullptr);

        std::string output = descriptor->getResult(nodeId);
        AIVision::deinit();

        rj::Document d;
        d.Parse(output.c_str());
        EXPECT_TRUE(d.IsObject());
        EXPECT_TRUE(d.HasMember("useExtraOutput"));
        EXPECT_EQ(d["useExtraOutput"].GetBool(), true);
    }

    void testPipeApi(const ExtraOutputs& extraOutputs) {
        cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
        std::string output;

        EdgeAIVision& ai = EdgeAIVision::getInstance();
        ai.startup();
        EXPECT_TRUE(ai.isStarted());
        EXPECT_TRUE(ai.pipeCreate(pipeId, pipeConfig));
        EXPECT_TRUE(ai.pipeDetect(pipeId, input, output, extraOutputs));
        EXPECT_TRUE(ai.pipeDelete(pipeId));
        ai.shutdown();

        rj::Document d;
        d.Parse(output.c_str());
        EXPECT_TRUE(d.IsObject());
        EXPECT_TRUE(d["results"].GetArray()[0][nodeId.c_str()].HasMember("useExtraOutput"));
        EXPECT_EQ(d["results"].GetArray()[0][nodeId.c_str()]["useExtraOutput"].GetBool(), true);
    }

};

TEST_F(ExtraOutputTest, floatDataTest)
{
    ExtraOutputTestDetector td;
    cv::Mat input = cv::imread(basePath + "/images/mona.jpg", cv::IMREAD_COLOR);
    std::unique_ptr<float[]> data{ new float[d.data_float.size()]};

    EXPECT_EQ(td.init(""), kAifOk);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Descriptor>();
    ExtraOutput extraOutput(
            ExtraOutputType::FLOAT_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(float) * d.data_float.size());
    descriptor->initExtraOutput(extraOutput);

    EXPECT_TRUE(td.detect(input, descriptor) == aif::kAifOk);
    float* outputData = static_cast<float*>(extraOutput.buffer());
    for (int i = 0; i < d.data_float.size(); i++) {
        EXPECT_TRUE(floatEquals(d.data_float[i], outputData[i]));
    }

    rj::Document d;
    d.Parse(descriptor->toStr().c_str());
    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("useExtraOutput"));
    EXPECT_EQ(d["useExtraOutput"].GetBool(), true);
}

TEST_F(ExtraOutputTest, uint8DataTest)
{
    ExtraOutputTestDetector td;
    cv::Mat input = cv::imread(basePath + "/images/mona.jpg", cv::IMREAD_COLOR);
    std::unique_ptr<uint8_t[]> data{ new uint8_t[d.data_uint8.size()]};

    EXPECT_EQ(td.init(""), kAifOk);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Descriptor>();
    ExtraOutput extraOutput(
            ExtraOutputType::UINT8_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(uint8_t) * d.data_uint8.size());
    descriptor->initExtraOutput(extraOutput);

    EXPECT_TRUE(td.detect(input, descriptor) == aif::kAifOk);
    uint8_t* outputData = static_cast<uint8_t*>(extraOutput.buffer());
    for (int i = 0; i < d.data_uint8.size(); i++) {
        EXPECT_EQ(d.data_uint8[i], outputData[i]);
    }
    rj::Document d;
    d.Parse(descriptor->toStr().c_str());
    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("useExtraOutput"));
    EXPECT_EQ(d["useExtraOutput"].GetBool(), true);
}

TEST_F(ExtraOutputTest, int8DataTest)
{
    ExtraOutputTestDetector td;
    cv::Mat input = cv::imread(basePath + "/images/mona.jpg", cv::IMREAD_COLOR);
    std::unique_ptr<int8_t[]> data{ new int8_t[d.data_int8.size()]};

    EXPECT_EQ(td.init(""), kAifOk);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Descriptor>();
    ExtraOutput extraOutput(
            ExtraOutputType::INT8_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(int8_t) * d.data_int8.size());
    descriptor->initExtraOutput(extraOutput);

    EXPECT_TRUE(td.detect(input, descriptor) == aif::kAifOk);
    int8_t* outputData = static_cast<int8_t*>(extraOutput.buffer());
    for (int i = 0; i < d.data_int8.size(); i++) {
        EXPECT_EQ(d.data_int8[i], outputData[i]);
    }
    rj::Document d;
    d.Parse(descriptor->toStr().c_str());
    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("useExtraOutput"));
    EXPECT_EQ(d["useExtraOutput"].GetBool(), true);
}

TEST_F(ExtraOutputTest, floatDataTestUsingFacadeAPI)
{
    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;
    std::unique_ptr<float[]> data{ new float[d.data_float.size()]};
    ExtraOutput extraOutput(
            ExtraOutputType::FLOAT_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(float) * d.data_float.size());
    auto type = EdgeAIVision::DetectorType::FACE;

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.createDetector(type, option));
    EXPECT_TRUE(ai.detect(type, input, output, extraOutput));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    float* outputData = static_cast<float*>(extraOutput.buffer());
    for (int i = 0; i < d.data_float.size(); i++) {
        EXPECT_TRUE(floatEquals(d.data_float[i], outputData[i]));
    }
    rj::Document d;
    d.Parse(output.c_str());
    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("useExtraOutput"));
    EXPECT_EQ(d["useExtraOutput"].GetBool(), true);
}

TEST_F(ExtraOutputTest, uint8DataTestUsingFacadeAPI)
{
    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;
    std::unique_ptr<uint8_t[]> data{ new uint8_t[d.data_uint8.size()]};
    ExtraOutput extraOutput(
            ExtraOutputType::UINT8_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(uint8_t) * d.data_uint8.size());
    auto type = EdgeAIVision::DetectorType::FACE;

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.createDetector(type, option));
    EXPECT_TRUE(ai.detect(type, input, output, extraOutput));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    uint8_t* outputData = static_cast<uint8_t*>(extraOutput.buffer());
    for (int i = 0; i < d.data_uint8.size(); i++) {
        EXPECT_EQ(d.data_uint8[i], outputData[i]);
    }
    rj::Document d;
    d.Parse(output.c_str());
    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("useExtraOutput"));
    EXPECT_EQ(d["useExtraOutput"].GetBool(), true);
}

TEST_F(ExtraOutputTest, int8DataTestUsingFacadeAPI)
{
    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;
    std::unique_ptr<int8_t[]> data{ new int8_t[d.data_int8.size()]};
    ExtraOutput extraOutput(
            ExtraOutputType::INT8_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(int8_t) * d.data_int8.size());
    auto type = EdgeAIVision::DetectorType::FACE;

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.createDetector(type, option));
    EXPECT_TRUE(ai.detect(type, input, output, extraOutput));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    int8_t* outputData = static_cast<int8_t*>(extraOutput.buffer());
    for (int i = 0; i < d.data_int8.size(); i++) {
        EXPECT_EQ(d.data_int8[i], outputData[i]);
    }
    rj::Document d;
    d.Parse(output.c_str());
    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("useExtraOutput"));
    EXPECT_EQ(d["useExtraOutput"].GetBool(), true);
}

TEST_F(ExtraOutputTest, floatDataTestPipe)
{
    std::unique_ptr<float[]> data{ new float[d.data_float.size()]};
    ExtraOutput extraOutput(
            ExtraOutputType::FLOAT_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(float) * d.data_float.size());
    ExtraOutputs extraOutputs;
    extraOutputs[nodeId] = extraOutput;

    testPipe(extraOutputs);

    float* outputData = static_cast<float*>(extraOutput.buffer());
    for (int i = 0; i < d.data_float.size(); i++) {
        EXPECT_TRUE(floatEquals(d.data_float[i], outputData[i]));
    }
}

TEST_F(ExtraOutputTest, uint8DataTestPipe)
{
    std::unique_ptr<uint8_t[]> data{ new uint8_t[d.data_uint8.size()]};
    ExtraOutput extraOutput(
            ExtraOutputType::UINT8_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(float) * d.data_uint8.size());
    ExtraOutputs extraOutputs;
    extraOutputs[nodeId] = extraOutput;

    testPipe(extraOutputs);

    uint8_t* outputData = static_cast<uint8_t*>(extraOutput.buffer());
    for (int i = 0; i < d.data_uint8.size(); i++) {
        EXPECT_EQ(d.data_uint8[i], outputData[i]);
    }
}

TEST_F(ExtraOutputTest, int8DataTestPipe)
{
    std::unique_ptr<int8_t[]> data{ new int8_t[d.data_int8.size()]};
    ExtraOutput extraOutput(
            ExtraOutputType::INT8_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(float) * d.data_int8.size());
    ExtraOutputs extraOutputs;
    extraOutputs[nodeId] = extraOutput;

    testPipe(extraOutputs);

    int8_t* outputData = static_cast<int8_t*>(extraOutput.buffer());
    for (int i = 0; i < d.data_int8.size(); i++) {
        EXPECT_EQ(d.data_int8[i], outputData[i]);
    }
}

TEST_F(ExtraOutputTest, floatDataTestPipeApi)
{
    std::unique_ptr<float[]> data{ new float[d.data_float.size()]};
    ExtraOutput extraOutput(
            ExtraOutputType::FLOAT_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(float) * d.data_float.size());
    ExtraOutputs extraOutputs;
    extraOutputs[nodeId] = extraOutput;

    testPipeApi(extraOutputs);

    float* outputData = static_cast<float*>(extraOutput.buffer());
    for (int i = 0; i < d.data_float.size(); i++) {
        EXPECT_TRUE(floatEquals(d.data_float[i], outputData[i]));
    }
}

TEST_F(ExtraOutputTest, uint8DataTestPipeApi)
{
    std::unique_ptr<uint8_t[]> data{ new uint8_t[d.data_uint8.size()]};
    ExtraOutput extraOutput(
            ExtraOutputType::UINT8_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(float) * d.data_uint8.size());
    ExtraOutputs extraOutputs;
    extraOutputs[nodeId] = extraOutput;

    testPipeApi(extraOutputs);

    uint8_t* outputData = static_cast<uint8_t*>(extraOutput.buffer());
    for (int i = 0; i < d.data_uint8.size(); i++) {
        EXPECT_EQ(d.data_uint8[i], outputData[i]);
    }
}

TEST_F(ExtraOutputTest, int8DataTestPipeApi)
{
    std::unique_ptr<int8_t[]> data{ new int8_t[d.data_int8.size()]};
    ExtraOutput extraOutput(
            ExtraOutputType::INT8_ARRAY,
            static_cast<void*>(data.get()),
            sizeof(float) * d.data_int8.size());
    ExtraOutputs extraOutputs;
    extraOutputs[nodeId] = extraOutput;

    testPipeApi(extraOutputs);

    int8_t* outputData = static_cast<int8_t*>(extraOutput.buffer());
    for (int i = 0; i < d.data_int8.size(); i++) {
        EXPECT_EQ(d.data_int8[i], outputData[i]);
    }
}
