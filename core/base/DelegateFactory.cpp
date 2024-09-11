/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/DelegateFactory.h>
#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>

namespace aif {

t_aif_status DelegateFactory::registerGenerator(
        const std::string& id,
        const DelegateGenerator& delegateGenerator)
{
    if (m_delegateGenerators.find(id) != m_delegateGenerators.end()) {
        Loge(id, " delegate generator is already registered");
        return kAifError;
    }

    m_delegateGenerators[id] = delegateGenerator;
    return kAifOk;
}

std::shared_ptr<Delegate> DelegateFactory::getDelegate(const std::string& id, const std::string& option)
{
    if (!AIVision::isInitialized()) {
        Loge(__func__, "AIVision is not initialized");
        return nullptr;
    }

    if (m_delegateGenerators.find(id) == m_delegateGenerators.end()) {
        Loge(id, " delegate generator is not registered");
        return nullptr;
    }

    auto range = m_delegates.equal_range(id);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second != nullptr && it->second->getOption() == option) {
            return it->second;
        }
    }

    auto delegate = m_delegateGenerators[id](option);
    if (!delegate) {
        Loge(__func__, " failed to generate delegate ", id, ", option: ", option);
        return nullptr;
    }

    auto itr = m_delegates.insert(std::make_pair(id, delegate));
    if (itr == m_delegates.end()) {
        Loge(__func__, " failed to insert m_delegates");
        return nullptr;
    }

    return itr->second;
}
} // end of idspace aif
