/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_MNAME_DETECTOR_H
#define AIF_CPU_MNAME_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/mname/MNameDescriptor.h>
#include <aif/mname/MNameDetector.h>

namespace aif {

class CpuMNameDetector : public MNameDetector {
  private:
    CpuMNameDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuMNameDetector();
};

DetectorFactoryRegistration<CpuMNameDetector, MNameDescriptor>
    mname_model_cpu("mname_model_cpu");

} // end of namespace aif

#endif // AIF_CPU_MNAME_DETECTOR_H
