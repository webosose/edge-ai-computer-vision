/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/pipe/Pipe.h>
#include <aif/facade/EdgeAIVision.h>
#include <aif/tools/Utils.h>

#define DEFAULT_FACE_MODEL "face_yunet_360_640"
#define DEFAULT_POSE_MODEL "posenet_mobilenet_cpu"
#define DEFAULT_SEGMENTATION_MODEL "selfie_mediapipe_cpu"
#define DEFAULT_TEXT_MODEL "text_paddleocr_320_v2"

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

    AIVision::init(basePath);
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
    std::string model;
    if (type == DetectorType::FACE) {
        model = DEFAULT_FACE_MODEL;
    } else if (type == DetectorType::POSE) {
        model = DEFAULT_POSE_MODEL;
    } else if (type == DetectorType::SEGMENTATION) {
        model = DEFAULT_SEGMENTATION_MODEL;
    } else if (type == DetectorType::TEXT) {
        model = DEFAULT_TEXT_MODEL;
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
    DetectorFactory::get().deleteDetector(m_selectedModels[type]);
    m_selectedModels.erase(type);
    return true;
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
    m_pipeMap[id] = pipe;
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

    m_pipeMap.erase(id);
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

} // namespace aif
