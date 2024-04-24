/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CHANGE_INPUT_OPERATION_H
#define CHANGE_INPUT_OPERATION_H

#include <aif/pipe/BridgeOperation.h>
#include <aif/pipe/ChangeInputOperationConfig.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>

namespace aif {

class ChangeInputOperation : public BridgeOperation
{
    private:
       ChangeInputOperation(const std::string& id);

    public:
       virtual ~ChangeInputOperation();
       bool runImpl(const std::shared_ptr<NodeInput>& input) override;
       template <typename T1, typename T2>
       friend class NodeOperationFactoryRegistration;
};

NodeOperationFactoryRegistration<ChangeInputOperation, ChangeInputOperationConfig>
    change_input_operation("change_input");

} // end of namespace aif

#endif // AIF_CHANGE_INPUT_OPERATION_H
