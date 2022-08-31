/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FITTV_POSE_DESCRIPTOR_REGISTRATION_H
#define AIF_FITTV_POSE_DESCRIPTOR_REGISTRATION_H

#include <aif/pipe/PipeDescriptorFactoryRegistration.h>
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>

namespace aif {

PipeDescriptorFactoryRegistration<FitTvPoseDescriptor>
    fittv_pose_descriptor("fittv_pose");

} // end of namespace aif

#endif
