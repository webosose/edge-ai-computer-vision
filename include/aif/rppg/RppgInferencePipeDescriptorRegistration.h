/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_RPPG_PIPE_INFERENCE_DESCRIPTOR_REGISTRATION_H
#define AIF_RPPG_PIPE_INFERENCE_DESCRIPTOR_REGISTRATION_H

#include <aif/pipe/PipeDescriptorFactoryRegistration.h>
#include <aif/rppg/RppgInferencePipeDescriptor.h>

namespace aif {

PipeDescriptorFactoryRegistration<RppgInferencePipeDescriptor>
    rppg_inference_pipe_descriptor("rppg_inference_pipe");

} // end of namespace aif

#endif // AIF_RPPG_PIPE_INFERENCE_DESCRIPTOR_REGISTRATION_H
