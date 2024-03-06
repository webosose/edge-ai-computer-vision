/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NODE_OPERATION_FACTORY_REGISTRATION_H
#define AIF_NODE_OPERATION_FACTORY_REGISTRATION_H

#include <aif/base/IRegistration.h>
#include <aif/log/Logger.h>
#include <aif/pipe/NodeOperation.h>
#include <aif/pipe/NodeOperationFactory.h>

#include <string>

namespace aif {

template <typename T1, typename T2>
class NodeOperationFactoryRegistration : public IRegistration
{
public:
    NodeOperationFactoryRegistration(const std::string& type) : IRegistration(type, kNodeOperation)
    {
        RegistrationJob::get().addRegistration(m_extension, m_id, this);
    }
    void doRegister() override
    {
        Logi("doRegister: ", m_id);
        if (kAifOk != NodeOperationFactory::get().registerGenerator(
             m_id,
             [](const std::string& id) {
                 std::shared_ptr<NodeOperation> operation(new T1(id));
                 return operation;
             },
             []() {
                 std::shared_ptr<NodeOperationConfig> config(new T2());
                 return config;
             }
        ) ) {
            return Loge("Failed to register NodeOperation: ", m_id);
        }
        Logi("Success to register NodeOperation: ", m_id);
    }

    ~NodeOperationFactoryRegistration()
    {
        auto& reg = RegistrationJob::get().getRegistration(m_extension, m_id);
        if (reg) {
            reg = nullptr;
            RegistrationJob::get().removeRegistration(m_extension, m_id);
        }
    }
};

} // end of namespace aif

#endif  // AIF_NODE_OPERATION_FACTORY_REGISTRATION_H
