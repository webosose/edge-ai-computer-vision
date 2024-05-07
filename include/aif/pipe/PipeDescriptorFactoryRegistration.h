/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PIPE_DESCRIPTOR_FACTORY_REGISTRATION_H
#define AIF_PIPE_DESCRIPTOR_FACTORY_REGISTRATION_H

#include <aif/pipe/PipeDescriptorFactory.h>
#include <aif/base/IRegistration.h>
#include <aif/log/Logger.h>

#include <string>

namespace aif {

template <typename T>
class PipeDescriptorFactoryRegistration : public IRegistration
{
public:
    PipeDescriptorFactoryRegistration(const std::string& id) : IRegistration(id, kPipeDescriptor)
    {
        RegistrationJob::get().addRegistration(m_extension, m_id, this);
    }

    void doRegister(std::string prefixId = "") override
    {
        Logi("doRegister: ", m_id);
        if (kAifOk != PipeDescriptorFactory::get().registerGenerator(
             prefixId + m_id,
             []() {
                 std::shared_ptr<PipeDescriptor> descriptor(new T());
                 return descriptor;
             }
        ) ) {
            return Loge("Failed to register PipeDescriptor: ", prefixId + m_id);
        }
        Logi("Success to register PipeDescriptor: ", prefixId + m_id);
    }

    ~PipeDescriptorFactoryRegistration()
    {
        auto& reg = RegistrationJob::get().getRegistration(m_extension, m_id);
        if (reg) {
            reg = nullptr;
            RegistrationJob::get().removeRegistration(m_extension, m_id);
        }
    }
};

} // end of namespace aif

#endif  // AIF_PIPE_DESCRIPTOR_FACTORY_REGISTRATION_H
