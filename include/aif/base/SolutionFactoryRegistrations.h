/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_SOLUTION_FACTORY_REGISTRATION_H
#define AIF_SOLUTION_FACTORY_REGISTRATION_H

#include <aif/base/Types.h>
#include <aif/base/SolutionFactory.h>
#include <aif/base/IRegistration.h>
#include <aif/log/Logger.h>

#include <string>

namespace aif {

template <typename T>
class SolutionFactoryRegistration : public IRegistration
{
public:
    SolutionFactoryRegistration(const std::string &id) : IRegistration(id, kSolution)
    {
        RegistrationJob::get().addRegistration(m_extension, m_id, this);
    }

    void doRegister(std::string prefixId = "") override
    {
        Logd("doRegister: ", prefixId + m_id);
        if (kAifOk != SolutionFactory::get().registerGenerator(
            prefixId + m_id,
            [](const std::string& name) {
                std::shared_ptr<EdgeAISolution> solution(new T(name));
                return solution;
            }
        )) {
            Loge("Failed to register solution: ", prefixId + m_id);
            return;
        }
        Logd("Success to register solution: ", prefixId + m_id);
    }

    ~SolutionFactoryRegistration()
    {
        auto& reg = RegistrationJob::get().getRegistration(m_extension, m_id);
        if (reg) {
            reg = nullptr;
            RegistrationJob::get().removeRegistration(m_extension, m_id);
        }
    }

};
}; // end of namespace aif

#endif // AIF_SOLUTION_FACTORY_REGISTRATION_H
