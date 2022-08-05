/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_BRIDGE_OPERATION_H
#define AIF_BRIDGE_OPERATION_H

#include <aif/pipe/NodeOperation.h>

namespace aif {

class BridgeOperation : public NodeOperation {
protected:
    BridgeOperation(const std::string& id)
        : NodeOperation(id) {}
    virtual ~BridgeOperation() {}
};

} // end of namespace aif

#endif // AIF_BRIDGE_OPERATION_H
