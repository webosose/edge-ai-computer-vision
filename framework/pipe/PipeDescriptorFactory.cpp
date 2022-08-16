/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/PipeDescriptorFactory.h>
#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

t_aif_status PipeDescriptorFactory::registerGenerator(
        const std::string& id,
        const PipeDescriptorGenerator& generator)
{
    try {
        if (m_generators.find(id) != m_generators.end()) {
            throw std::runtime_error(id + " operation generator is already registered");
        }
     } catch (const std::exception& e) {
        Loge(__func__, e.what());
        return kAifError;
    } catch (...) {
        Loge("Unknown exception occured!!");
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

    try {
        if (m_generators.find(id) == m_generators.end()) {
            Logi("create default pipe descriptor");
            return std::make_shared<PipeDescriptor>();
        }
        Logi("create node descriptor: ", id);
        std::shared_ptr<PipeDescriptor> descriptor= m_generators[id]();
        return descriptor;
    } catch (const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return nullptr;
    } catch (...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return nullptr;
    }
}

} // end of namespace aif
