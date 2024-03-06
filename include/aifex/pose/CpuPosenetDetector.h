/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_POSENET_DETECTOR_H
#define AIF_CPU_POSENET_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aifex/pose/PosenetDescriptor.h>
#include <aifex/pose/PosenetDetector.h>

namespace aif {

class CpuPosenetDetector : public PosenetDetector {
  private:
    CpuPosenetDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuPosenetDetector();

  protected:
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override;
};

DetectorFactoryRegistration<CpuPosenetDetector, PosenetDescriptor>
    posenet_mobilenet_cpu("posenet_mobilenet_cpu");

} // end of namespace aif

#endif // AIF_CPU_POSENET_DETECTOR_H
