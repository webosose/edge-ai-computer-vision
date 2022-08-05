/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NODE_OPERATION_FACTORY_REGISTRATION_H
#define AIF_NODE_OPERATION_FACTORY_REGISTRATION_H

#include <aif/pipe/NodeOperation.h>
#include <aif/pipe/NodeOperationFactory.h>

#include <string>

namespace aif {

template <typename T1, typename T2>
class NodeOperationFactoryRegistration
{
public:
    NodeOperationFactoryRegistration(const std::string& type) {
        NodeOperationFactory::get().registerGenerator(
             type,
             [](const std::string& id) {
                 std::shared_ptr<NodeOperation> operation(new T1(id));
                 return operation;
             },
             []() {
                 std::shared_ptr<NodeOperationConfig> config(new T2());
                 return config;
             }
        );
    }
};

} // end of namespace aif

#endif  // AIF_NODE_OPERATION_FACTORY_REGISTRATION_H
