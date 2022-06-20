/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/DetectorFactory.h>
#include <aif/base/AIVision.h>
#include <aif/base/DelegateFactory.h>
#include <aif/log/Logger.h>

namespace aif {

t_aif_status DetectorFactory::registerGenerator(
        const std::string& id,
        const DetectorGenerator& detectorGenerator,
        const DescriptorGenerator& descriptorGenerator)
{
    try {
        if (m_detectorGenerators.find(id) != m_detectorGenerators.end()) {
            throw std::runtime_error(id + " detector generator is already registered");
        }
        if (m_descriptorGenerators.find(id) != m_descriptorGenerators.end()) {
            throw std::runtime_error(id + " descriptor generator is already registered");
        }
    } catch (const std::exception& e) {
        std::cerr << __func__ << " Error: " << e.what() << std::endl;
        return kAifError;
    } catch (...) {
        std::cerr << __func__ << " Error: Unknown exception occured!!\n";
        return kAifError;
    }

    m_detectorGenerators[id] = detectorGenerator;
    m_descriptorGenerators[id] = descriptorGenerator;
    return kAifOk;
}

void DetectorFactory::deleteDetector(const std::string& id)
{
    m_detectors.erase(id);
}

std::shared_ptr<Detector> DetectorFactory::getDetector(const std::string& id, const std::string& param)
{
    if (!AIVision::isInitialized()) {
        Loge(__func__, "AIVision is not initialized");
        return nullptr;
    }

    try {
        if (m_detectors.find(id) == m_detectors.end() &&
            m_detectorGenerators.find(id) == m_detectorGenerators.end()) {
            throw std::runtime_error(id + " detector generator is not registered");
        }
        if (m_detectors.find(id) != m_detectors.end()) {
            return m_detectors[id];
        }
        m_detectors[id] = m_detectorGenerators[id]();
        if (m_detectors[id]->init(param) != kAifOk) {
            throw std::runtime_error("detector init error");
        }
        return m_detectors[id];

    } catch (const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return nullptr;
    } catch (...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return nullptr;
    }
}

std::shared_ptr<Descriptor> DetectorFactory::getDescriptor(const std::string& id) {
    try {
        if (m_descriptorGenerators.find(id) == m_descriptorGenerators.end()) {
            throw std::runtime_error(id + " descriptor generator is not registered");
        }
        return m_descriptorGenerators[id]();
    } catch (const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return nullptr;
    } catch (...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return nullptr;
    }
}

void DetectorFactory::clear()
{
    DelegateFactory::get().clear();
    m_detectors.clear();
}

} // end of idspace aif
