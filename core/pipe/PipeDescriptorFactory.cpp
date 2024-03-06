/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/PipeDescriptorFactory.h>
#include <aif/base/AIVision.h>
#include <aif/base/ExtensionLoader.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

t_aif_status PipeDescriptorFactory::registerGenerator(
        const std::string& id,
        const PipeDescriptorGenerator& generator)
{
    if (m_generators.find(id) != m_generators.end()) {
        Loge(id + " operation generator is already registered");
        return kAifError;
    }

    m_generators[id] = generator;
    return kAifOk;
}

std::shared_ptr<PipeDescriptor> PipeDescriptorFactory::create(
        const std::string& id)
{
    if (!AIVision::isInitialized()) {
        Loge(__func__, "AIVision is not initialized");
        return nullptr;
    }

    if (m_generators.find(id) == m_generators.end()) {
        if (ExtensionLoader::get().isAvailable(id, kPipeDescriptor) != kAifOk) {
            Logd("create default pipe descriptor");
            return std::make_shared<PipeDescriptor>();
        }
        if (ExtensionLoader::get().enableFeature(id, kPipeDescriptor) != kAifOk) {
            Logd(id, " ExtensionLoader enableFeature error. create default pipe descriptor");
            return std::make_shared<PipeDescriptor>();
        }
    }
    Logd("create node descriptor: ", id);
    std::shared_ptr<PipeDescriptor> descriptor = m_generators[id]();
    return descriptor;
}

void PipeDescriptorFactory::clear()
{
    m_generators.clear();
}

} // end of namespace aif
