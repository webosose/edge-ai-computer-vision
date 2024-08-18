/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/SolutionFactory.h>
#include <aif/pipe/Pipe.h>
#include <aif/facade/EdgeAIVision.h>
#include <aif/tools/Utils.h>
#include <fstream>
#include <aif/tools/PerformanceReporter.h>

#define DEFAULT_FACE_MODEL "face_yunet_360_640"
#define DEFAULT_POSE_MODEL "posenet_mobilenet_cpu"
#define DEFAULT_SEGMENTATION_MODEL "selfie_mediapipe_cpu"
#define DEFAULT_TEXT_MODEL "text_paddleocr_320_v2"
#define DEFAULT_SIGNLANGUAGEAREA_MODEL "signlanguagearea_model_cpu"

namespace aif {
namespace rj = rapidjson;

std::unique_ptr<EdgeAIVision> EdgeAIVision::s_instance;
std::once_flag EdgeAIVision::s_onceFlag;

EdgeAIVision &EdgeAIVision::getInstance() {
    std::call_once(EdgeAIVision::s_onceFlag,
                   []() { s_instance.reset(new EdgeAIVision); });

    return *(s_instance.get());
}

bool EdgeAIVision::isStarted() const { return AIVision::isInitialized(); }

bool EdgeAIVision::startup(const std::string &basePath) {

    std::lock_guard<std::mutex> lock(m_mutex);
    if (AIVision::isInitialized())
        return false;

    if (!AIVision::init(basePath))
        return false;

    return isStarted();
}

bool EdgeAIVision::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!AIVision::isInitialized())
        return false;

    m_pipeMap.clear();
    m_selectedModels.clear();

    AIVision::deinit();
    return !isStarted();
}

std::string EdgeAIVision::getDefaultModel(DetectorType type) const {
    std::string model = "";
    if (type == DetectorType::FACE) {
        model = DEFAULT_FACE_MODEL;
    } else if (type == DetectorType::POSE) {
        model = DEFAULT_POSE_MODEL;
    } else if (type == DetectorType::SEGMENTATION) {
        model = DEFAULT_SEGMENTATION_MODEL;
    } else if (type == DetectorType::TEXT) {
        model = DEFAULT_TEXT_MODEL;
    } else if (type == DetectorType::SIGNLANGUAGEAREA) {
        model = DEFAULT_SIGNLANGUAGEAREA_MODEL;
    }
    return model;
}

bool EdgeAIVision::createDetector(DetectorType type,
                                  const std::string &option) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }
    if (m_selectedModels.find(type) != m_selectedModels.end()) {
        Logw("Detector is already created : ", static_cast<int>(type));
        return false;
    }
    std::string model = getDefaultModel(type);
    std::string param;

    if (!option.empty()) {
        rj::Document json;
        rj::ParseResult ok = json.Parse(option.c_str());
        if (ok) {
            if (json.IsObject() && json.HasMember("model")) {
                // CID 9333370
                model = (json["model"].GetString() != nullptr ? json["model"].GetString() : "");
            }
            if (json.IsObject() && json.HasMember("param")) {
                param = jsonObjectToString(json["param"]);
            }
        }
    }
    if (type == DetectorType::CUSTOM && model.empty()) {
        Loge("To use Custom DetectorType, option should have 'model' field");
        return false;
    }
    m_selectedModels[type] = model;
    auto detector = DetectorFactory::get().getDetector(model, param);
    return (detector != nullptr);
}

bool EdgeAIVision::deleteDetector(DetectorType type) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }
    if (m_selectedModels.find(type) == m_selectedModels.end()) {
        Logw("Detector is not created : ", static_cast<int>(type));
        return false;
    }
    PerformanceReporter::get().showSimpleReport();
    DetectorFactory::get().deleteDetector(m_selectedModels[type]);
    m_selectedModels.erase(type);
    return true;
}

bool EdgeAIVision::updateDetector(DetectorType type, const std::string& option)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }
    if (m_selectedModels.find(type) == m_selectedModels.end()) {
        Logw("Detector is not created : ", static_cast<int>(type));
        return false;
    }

    std::string model = m_selectedModels[type];
    auto detector = DetectorFactory::get().getDetector(model);
    if (detector == nullptr) {
        Loge("Detector get failed: ", model);
        return false;
    }

    std::string param = "";
    if (!option.empty()) {
        rj::Document json;
        rj::ParseResult ok = json.Parse(option.c_str());
        if (ok) {
            if (json.IsObject() && json.HasMember("param")) {
                param = jsonObjectToString(json["param"]);
                auto res = detector->updateConfig(param);
                return (kAifOk == res);
            }
        }
    }

    Loge("cannot parse option: " , option);
    return false;
}

bool EdgeAIVision::detect(DetectorType type, const cv::Mat &input,
                          std::string &output) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }

    if (m_selectedModels.find(type) == m_selectedModels.end()) {
        Loge("Detector is not created : ", static_cast<int>(type));
        return false;
    }

    std::string model = m_selectedModels[type];
    auto detector = DetectorFactory::get().getDetector(model);
    auto descriptor = DetectorFactory::get().getDescriptor(model);

    if (detector == nullptr) {
        Loge("Detector get failed: ", model);
        return false;
    }

    if (descriptor == nullptr) {
        Loge("Descriptor get failed: ", model);
        return false;
    }

    auto res = detector->detect(input, descriptor);
    descriptor->addReturnCode(res);
    output = descriptor->toStr();

    return (kAifOk == res);
}

bool EdgeAIVision::detectFromFile(DetectorType type,
                                  const std::string &inputPath,
                                  std::string &output) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }

    if (m_selectedModels.find(type) == m_selectedModels.end()) {
        Loge("Detector is not created : ", static_cast<int>(type));
        return false;
    }

    std::string model = m_selectedModels[type];
    auto detector = DetectorFactory::get().getDetector(model);
    auto descriptor = DetectorFactory::get().getDescriptor(model);

    if (detector == nullptr) {
        Loge("Detector get failed: ", model);
        return false;
    }

    if (descriptor == nullptr) {
        Loge("Descriptor get failed: ", model);
        return false;
    }

    auto res = detector->detectFromImage(inputPath, descriptor);
    descriptor->addReturnCode(res);
    output = descriptor->toStr();

    return (kAifOk == res);
}

bool EdgeAIVision::detectFromBase64(DetectorType type, const std::string &input,
                                    std::string &output) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }

    if (m_selectedModels.find(type) == m_selectedModels.end()) {
        Loge("Detector is not created : ", static_cast<int>(type));
        return false;
    }

    std::string model = m_selectedModels[type];
    auto detector = DetectorFactory::get().getDetector(model);
    auto descriptor = DetectorFactory::get().getDescriptor(model);

    if (detector == nullptr) {
        Loge("Detector get failed: ", model);
        return false;
    }

    if (descriptor == nullptr) {
        Loge("Descriptor get failed: ", model);
        return false;
    }
    auto res = detector->detectFromBase64(input, descriptor);
    descriptor->addReturnCode(res);
    output = descriptor->toStr();

    return (kAifOk == res);
}

bool EdgeAIVision::detect(DetectorType type, const cv::Mat &input,
                          std::string &output, ExtraOutput& extraOutput) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }

    if (m_selectedModels.find(type) == m_selectedModels.end()) {
        Loge("Detector is not created : ", static_cast<int>(type));
        return false;
    }

    std::string model = m_selectedModels[type];
    auto detector = DetectorFactory::get().getDetector(model);
    auto descriptor = DetectorFactory::get().getDescriptor(model);

    if (detector == nullptr) {
        Loge("Detector get failed: ", model);
        return false;
    }

    if (descriptor == nullptr) {
        Loge("Descriptor get failed: ", model);
        return false;
    }

    descriptor->initExtraOutput(extraOutput);
    auto res = detector->detect(input, descriptor);
    descriptor->addReturnCode(res);
    output = descriptor->toStr();

    return (kAifOk == res);
}

bool EdgeAIVision::pipeCreate(const std::string& id, const std::string& option)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }

    if (m_pipeMap.find(id) != m_pipeMap.end()) {
        Loge(id, ": pipe is already created");
        return false;
    }

    std::shared_ptr<Pipe> pipe = std::make_shared<Pipe>();
    if (!pipe->build(option))  {
        return false;
    }
    m_pipeMap[id] = std::move(pipe);
    return true;
}

bool EdgeAIVision::pipeDelete(const std::string& id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }

    if (m_pipeMap.find(id) == m_pipeMap.end()) {
        Loge(id, ": pipe is not created");
        return false;
    }
    PerformanceReporter::get().showSimpleReport();
    m_pipeMap.erase(id);
    return true;
}

bool EdgeAIVision::pipeUpdate(const std::string& id, const std::string& option)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }

    auto pipe = m_pipeMap.find(id);
    if (pipe == m_pipeMap.end()) {
        Loge(id, ": pipe is not created");
        return false;
    }

    auto pp = pipe->second;
    if (!pp->rebuild(option)) {
        return false;
    }

    return true;
}

bool EdgeAIVision::pipeDetect(
        const std::string& id, const cv::Mat& image, std::string& output)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }

   if (m_pipeMap.find(id) == m_pipeMap.end()) {
        Loge(id, ": pipe is not created");
        return false;
    }
    std::shared_ptr<Pipe> pipe = m_pipeMap[id];
    if (!pipe) {
        Loge(id, ": pipe is null");
        return false;
    }
    auto res = pipe->detect(image) ? kAifOk : kAifError;
    auto descriptor = pipe->getDescriptor();
    if (!descriptor) {
        Loge(id, ": pipe descriptor is null");
        return false;
    }
    descriptor->addReturnCode(res);
    output = descriptor->getResult();
    return (res == kAifOk);
}

bool EdgeAIVision::pipeDetect(
            const std::string& id,
            const cv::Mat& input,
            std::string& output,
            const ExtraOutputs& extraOutputs)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }

   if (m_pipeMap.find(id) == m_pipeMap.end()) {
        Loge(id, ": pipe is not created");
        return false;
    }
    std::shared_ptr<Pipe> pipe = m_pipeMap[id];
    if (!pipe) {
        Loge(id, ": pipe is null");
        return false;
    }
    auto res = pipe->detect(input, extraOutputs) ? kAifOk : kAifError;
    auto descriptor = pipe->getDescriptor();
    if (!descriptor) {
        Loge(id, ": pipe descriptor is null");
        return false;
    }
    descriptor->addReturnCode(res);
    output = descriptor->getResult();
    return (res == kAifOk);
}

bool EdgeAIVision::pipeDetectFromFile(
        const std::string& id, const std::string& imagePath, std::string& output)
{
   std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }

    if (m_pipeMap.find(id) == m_pipeMap.end()) {
        Loge(id, ": pipe is not created");
        return false;
    }
    std::shared_ptr<Pipe> pipe = m_pipeMap[id];
    if (!pipe) {
        Loge(id, ": pipe is null");
        return false;
    }
    auto res = pipe->detectFromFile(imagePath) ? kAifOk : kAifError;
    auto descriptor = pipe->getDescriptor();
    if (!descriptor) {
        Loge(id, ": pipe descriptor is null");
        return false;
    }
    descriptor->addReturnCode(res);
    output = descriptor->getResult();
    return (res == kAifOk);
}

bool EdgeAIVision::pipeDetectFromBase64(
        const std::string& id, const std::string& base64Image, std::string& output)
{
   std::lock_guard<std::mutex> lock(m_mutex);
    if (!isStarted()) {
        Loge("Edge AI Vision is not started");
        return false;
    }

    if (m_pipeMap.find(id) == m_pipeMap.end()) {
        Loge(id, ": pipe is not created");
        return false;
    }
    std::shared_ptr<Pipe> pipe = m_pipeMap[id];
    if (!pipe) {
        Loge(id, ": pipe is null");
        return false;
    }
    auto res =  pipe->detectFromBase64(base64Image) ? kAifOk : kAifError;
;
    auto descriptor = pipe->getDescriptor();
    if (!descriptor) {
        Loge(id, ": pipe descriptor is null");
        return false;
    }
    descriptor->addReturnCode(res);
    output = descriptor->getResult();
    return (res == kAifOk);
}

std::vector<std::string> EdgeAIVision::getCapableSolutionList()
{
    return SolutionFactory::get().getCapableSolutions();
}

EdgeAISolution::SolutionConfig EdgeAIVision::getDefaultSolutionConfig() {
    EdgeAISolution::SolutionConfig config;

    config.roi_region = {0,0,0,0};
    config.targetFps = -1;
    config.priority = EdgeAISolution::SolutionPriority::PRIORITY_NONE;
    config.numMaxPerson = 1;

    return config;
}

std::string EdgeAIVision::generateAIFParam(const std::string &solutionName)
{
    return generateAIFParam(solutionName, getDefaultSolutionConfig());
}

std::string EdgeAIVision::generateAIFParam(const std::string &solutionName, const std::string &outputPath)
{
    if (outputPath.empty()) {
        return generateAIFParam(solutionName);
    }

    return generateAIFParam(solutionName, getDefaultSolutionConfig(), outputPath);
}

std::string EdgeAIVision::generateAIFParam(const std::string &solutionName, EdgeAISolution::SolutionConfig config, const std::string &outputPath)
{
    std::shared_ptr<EdgeAISolution> solution = SolutionFactory::get().getSolution(solutionName);

    if (!solution) {
        Loge(__func__, solutionName, " : solution is not supported");
        return solutionName + " is not supported";
    }

    if (!outputPath.empty()) {
        auto res = solution->makeSolutionConfig(std::move(config));
        std::ofstream fout;
        fout.open(outputPath);
        fout << res << std::endl;
        fout.close();

        return res;
    }

    return solution->makeSolutionConfig(std::move(config));
}

} // namespace aif
