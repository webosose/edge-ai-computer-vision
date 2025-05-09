/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_SELFIE_DETECTOR_H
#define AIF_CPU_SELFIE_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aifex/selfie/SelfieDescriptor.h>
#include <aifex/selfie/SelfieDetector.h>

namespace aif {

class CpuSelfieDetector : public SelfieDetector {
  private:
    CpuSelfieDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuSelfieDetector();

  protected:
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override;
};

DetectorFactoryRegistration<CpuSelfieDetector, SelfieDescriptor>
    selfie_mediapipe_cpu("selfie_mediapipe_cpu");
} // end of namespace aif

#endif // AIF_CPU_SELFIE_DETECTOR_H
