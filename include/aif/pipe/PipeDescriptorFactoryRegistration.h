/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PIPE_DESCRIPTOR_FACTORY_REGISTRATION_H
#define AIF_PIPE_DESCRIPTOR_FACTORY_REGISTRATION_H

#include <aif/pipe/PipeDescriptorFactory.h>

#include <string>

namespace aif {

template <typename T>
class PipeDescriptorFactoryRegistration
{
public:
    PipeDescriptorFactoryRegistration(const std::string& id) {
        PipeDescriptorFactory::get().registerGenerator(
             id,
             []() {
                 std::shared_ptr<PipeDescriptor> descriptor(new T());
                 return descriptor;
             }
        );
    }
};

} // end of namespace aif

#endif  // AIF_PIPE_DESCRIPTOR_FACTORY_REGISTRATION_H
