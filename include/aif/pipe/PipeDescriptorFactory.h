/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PIPE_DESCRIPTOR_FACTORY_H
#define AIF_PIPE_DESCRIPTOR_FACTORY_H

#include <aif/base/Types.h>
#include <aif/pipe/PipeDescriptor.h>

#include <string>
#include <unordered_map>

namespace aif {

using PipeDescriptorGenerator =
    std::shared_ptr<PipeDescriptor>(*)();

class PipeDescriptorFactory
{
public:
    static PipeDescriptorFactory& get() {
        static PipeDescriptorFactory instance;
        return instance;
    }

    t_aif_status registerGenerator(
            const std::string& id,
            const PipeDescriptorGenerator& generator);

    std::shared_ptr<PipeDescriptor> create(const std::string& id);

    std::vector<std::string> getDescriptorIds() const {
        std::vector<std::string> ids;
        for (const auto& kv : m_generators) {
            ids.push_back(kv.first);
        }
        return ids;
    }

    void clear();

private:
    PipeDescriptorFactory() {}
    PipeDescriptorFactory(const PipeDescriptorFactory&) {}
    ~PipeDescriptorFactory() {}

private:
    std::unordered_map<std::string, PipeDescriptorGenerator> m_generators;
};

} // end of namespace aif

#endif  // AIF_PIPE_DESCRIPTOR_FACTORY_H
