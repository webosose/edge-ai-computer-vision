/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_DETECTOR_FACTORY_REGISTRATION_H
#define AIF_DETECTOR_FACTORY_REGISTRATION_H

#include <aif/base/Types.h>
#include <aif/base/Detector.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/IRegistration.h>
#include <aif/log/Logger.h>

#include <string>

namespace aif {

template <typename T1, typename T2>
class DetectorFactoryRegistration : public IRegistration
{
public:
    DetectorFactoryRegistration(const std::string& id) : IRegistration(id, kDetector)
    {
        RegistrationJob::get().addRegistration(m_extension, m_id, this);
    }

    void doRegister(std::string prefixId = "") override
    {
        Logi("doRegister: ", prefixId + m_id);
        if (kAifOk != DetectorFactory::get().registerGenerator(
                prefixId + m_id,
                []() {
                    std::shared_ptr<Detector> detector(new T1());
                    return detector;
                },
                []() {
                    std::shared_ptr<Descriptor> descriptor(new T2());
                    return descriptor;
                }
        ) ) {
            return Loge("Failed to register detector: ", prefixId + m_id);
        }
        Logi("Success to register detector: ", prefixId + m_id);
    }

    ~DetectorFactoryRegistration()
    {
        auto& reg = RegistrationJob::get().getRegistration(m_extension, m_id);
        if (reg) {
            reg = nullptr;
            RegistrationJob::get().removeRegistration(m_extension, m_id);
        }
    }
};

} // end of namespace aif

#endif  // AIF_DETECTOR_FACTORY_REGISTRATION_H
