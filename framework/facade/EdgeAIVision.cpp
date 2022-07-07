/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/facade/EdgeAIVision.h>
#include <aif/tools/Utils.h>

#define DEFAULT_FACE_MODEL "face_yunet_cpu"
#define DEFAULT_POSE_MODEL "posenet_mobilenet_cpu"
#define DEFAULT_SEGMENTATION_MODEL "selfie_mediapipe_cpu"

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
    if (AIVision::isInitialized())
        return false;

    AIVision::init(basePath);
    return isStarted();
}

bool EdgeAIVision::shutdown() {
    if (!AIVision::isInitialized())
        return false;

    DetectorFactory::get().clear();

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
    }
    return model;
}

bool EdgeAIVision::createDetector(DetectorType type,
                                  const std::string &option) {
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

    rj::Document json;
    rj::ParseResult ok = json.Parse(option.c_str());
    if (ok) {
        if (json.HasMember("model")) {
            model = json["model"].GetString();
        }
        if (json.HasMember("param")) {
            param = jsonObjectToString(json["param"]);
        }
    }
    m_selectedModels[type] = model;
    auto detector = DetectorFactory::get().getDetector(model, param);
    return (detector != nullptr);
}

bool EdgeAIVision::deleteDetector(DetectorType type) {
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

    auto res = detector->detect(input, descriptor);
    descriptor->addReturnCode(res);
    output = descriptor->toStr();

    return (kAifOk == res);
}

bool EdgeAIVision::detectFromFile(DetectorType type,
                                  const std::string &inputPath,
                                  std::string &output) {
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

    auto res = detector->detectFromImage(inputPath, descriptor);
    descriptor->addReturnCode(res);
    output = descriptor->toStr();

    return (kAifOk == res);
}

bool EdgeAIVision::detectFromBase64(DetectorType type, const std::string &input,
                                    std::string &output) {
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

    auto res = detector->detectFromBase64(input, descriptor);
    descriptor->addReturnCode(res);
    output = descriptor->toStr();

    return (kAifOk == res);
}

} // namespace aif
