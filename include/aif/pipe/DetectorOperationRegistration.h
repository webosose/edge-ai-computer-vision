/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_DETECTOR_OPERATION_REGISTRATION_H
#define AIF_DETECTOR_OPERATION_REGISTRATION_H

#include <aif/pipe/NodeOperationFactoryRegistration.h>

namespace aif {

NodeOperationFactoryRegistration<DetectorOperation, DetectorOperationConfig>
    detector_operation("detector");

} // end of namespace aif

#endif // AIF_DETECTOR_OPERATION_REGISTRATION_H
