/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/SolutionFactory.h>
#include <aif/log/Logger.h>
#include <aif/base/AIVision.h>
#include <aif/base/ExtensionLoader.h>
#include <iostream>

namespace aif {

t_aif_status SolutionFactory::registerGenerator(
            const std::string& id,
            const SolutionGenerator& solutionGenerator)
{
    if (m_solutionGenerators.find(id) != m_solutionGenerators.end()) {
        Loge(id, " solution generator is already registered");
        return kAifError;
    }

    m_solutionGenerators[id] = solutionGenerator;
    return kAifOk;
}

void SolutionFactory::deleteSolution(const std::string& id)
{
    m_solutions.erase(id);
}

std::shared_ptr<EdgeAISolution> SolutionFactory::getSolution(const std::string& id)
{
    if (!AIVision::isInitialized()) {
        std::cout << " AIVision is not initialized\n";
        return nullptr;
    }

    if (m_solutions.find(id) == m_solutions.end() &&
            m_solutionGenerators.find(id) == m_solutionGenerators.end()) {
        if (ExtensionLoader::get().isAvailable(id, kSolution) != kAifOk) {
            Loge(id, " solution generator is not registered");
            return nullptr;
        }
        if (ExtensionLoader::get().enableFeature(id, kSolution) != kAifOk) {
            Loge(id, " ExtensionLoader enableFeature error");
            return nullptr;
        }
    }

    if (m_solutions.find(id) != m_solutions.end()) {
        return m_solutions[id];
    }

    /* if not created, generate it! */
    m_solutions[id] = m_solutionGenerators[id](id);

    return m_solutions[id];
}

const std::vector<std::string> SolutionFactory::getCapableSolutions()
{
    std::vector<std::string> list;

    auto features = ExtensionLoader::get().getFeatureInfos();
    for (auto &feature : features) {
        if (feature.type == kSolution) {
            Logi(" found! : ", feature.name, " - " , feature.pluginName, " type: ", feature.type);
            list.push_back(feature.name);
        }
    }

    return list;
}

void SolutionFactory::clear()
{
    m_solutionGenerators.clear();
    m_solutions.clear();
}

} // end of namespace aif
