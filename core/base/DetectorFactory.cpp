/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/DetectorFactory.h>
#include <aif/base/AIVision.h>
#include <aif/base/DelegateFactory.h>
#include <aif/base/ExtensionLoader.h>
#include <aif/log/Logger.h>

namespace aif {

t_aif_status DetectorFactory::registerGenerator(
        const std::string& id,
        const DetectorGenerator& detectorGenerator,
        const DescriptorGenerator& descriptorGenerator)
{
    if (m_detectorGenerators.find(id) != m_detectorGenerators.end()) {
        Loge(id, " detector generator is already registered");
        return kAifError;
    }
    if (m_descriptorGenerators.find(id) != m_descriptorGenerators.end()) {
        Loge(id, " descriptor generator is already registered");
        return kAifError;
    }

    m_detectorGenerators[id] = detectorGenerator;
    m_descriptorGenerators[id] = descriptorGenerator;
    return kAifOk;
}

void DetectorFactory::deleteDetector(const std::string& id)
{
    if (m_detectors.find(id) != m_detectors.end()) {
        m_detectors.erase(id);
    }
}

std::shared_ptr<Detector> DetectorFactory::getDetector(const std::string& id, const std::string& param)
{
    if (!AIVision::isInitialized()) {
        Loge(__func__, "AIVision is not initialized");
        return nullptr;
    }

    if (m_detectors.find(id) == m_detectors.end() &&
            m_detectorGenerators.find(id) == m_detectorGenerators.end()) {
        if (ExtensionLoader::get().isAvailable(id, kDetector) != kAifOk) {
            Loge(id, " detector generator is not registered");
            return nullptr;
        }
        if (ExtensionLoader::get().enableFeature(id, kDetector) != kAifOk) {
            Loge(id, " ExtensionLoader enableFeature error");
            return nullptr;
        }
    }
    if (m_detectors.find(id) != m_detectors.end()) {
        return m_detectors[id];
    }

    m_detectors[id] = m_detectorGenerators[id]();
    if (m_detectors[id]->init(param) != kAifOk) {
        Loge(id, " detector init error");
        return nullptr;
    }
    return m_detectors[id];
}

std::shared_ptr<Descriptor> DetectorFactory::getDescriptor(const std::string& id) {
    if (m_descriptorGenerators.find(id) == m_descriptorGenerators.end()) {
        if (ExtensionLoader::get().isAvailable(id, kDetector) != kAifOk) {
            Loge(id, " descriptor generator is not registered");
            return nullptr;
        }
        if (ExtensionLoader::get().enableFeature(id, kDetector) != kAifOk) {
            Loge(id, " ExtensionLoader enableFeature error");
            return nullptr;
        }
    }
    return m_descriptorGenerators[id]();
}

void DetectorFactory::clear()
{
    // DelegateFactory::get().clear();
    m_detectorGenerators.clear();
    m_descriptorGenerators.clear();
    m_detectors.clear();
}

} // end of idspace aif
