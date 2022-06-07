/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_PALM_DETECTOR_H
#define AIF_CPU_PALM_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/palm/PalmDescriptor.h>
#include <aif/palm/PalmDetector.h>

namespace aif {

class CpuPalmDetector : public PalmDetector {
  protected:
    CpuPalmDetector(const std::string &modelPath);

  public:
    virtual ~CpuPalmDetector();
};

class LiteCpuPalmDetector : public CpuPalmDetector {
  private:
    LiteCpuPalmDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~LiteCpuPalmDetector() {}
};
DetectorFactoryRegistration<LiteCpuPalmDetector, PalmDescriptor>
    palm_lite_cpu("palm_lite_cpu");

class FullCpuPalmDetector : public CpuPalmDetector {
  private:
    FullCpuPalmDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~FullCpuPalmDetector() {}
};
DetectorFactoryRegistration<FullCpuPalmDetector, PalmDescriptor>
    palm_full_cpu("palm_full_cpu");

} // end of namespace aif

#endif // AIF_CPU_PALM_DETECTOR_H
