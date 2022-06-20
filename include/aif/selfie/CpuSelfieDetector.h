/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_SELFIE_DETECTOR_H
#define AIF_CPU_SELFIE_DETECTOR_H

#include <aif/selfie/SelfieDetector.h>
#include <aif/selfie/SelfieDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>

namespace aif {

class CpuSelfieDetector : public SelfieDetector
{
private:
    CpuSelfieDetector();

public:
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~CpuSelfieDetector();

protected:
    t_aif_status compileModel() override;

};

DetectorFactoryRegistration<CpuSelfieDetector, SelfieDescriptor>
selfie_mediapipe_cpu("selfie_mediapipe_cpu");
} // end of namespace aif

#endif // AIF_CPU_SELFIE_DETECTOR_H
