/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_RPPG_DETECTOR_H
#define AIF_CPU_RPPG_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/rppg/RppgDescriptor.h>
#include <aif/rppg/RppgDetector.h>

namespace aif {

class CpuRppgDetector : public RppgDetector {
  private:
    CpuRppgDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuRppgDetector();

  protected:
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override;
};

DetectorFactoryRegistration<CpuRppgDetector, RppgDescriptor>
    rppg_cpu("rppg_cpu");

} // end of namespace aif

#endif // AIF_CPU_RPPG_DETECTOR_H
