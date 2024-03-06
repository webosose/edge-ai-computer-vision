/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_HANDLANDMARK_DETECTOR_H
#define AIF_CPU_HANDLANDMARK_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aifex/handLandmark/HandLandmarkDescriptor.h>
#include <aifex/handLandmark/HandLandmarkDetector.h>

namespace aif {

class CpuHandLandmarkDetector : public HandLandmarkDetector {
  protected:
    CpuHandLandmarkDetector(const std::string &modelPath);

  public:
    virtual ~CpuHandLandmarkDetector();
};

class LiteCpuHandLandmarkDetector : public CpuHandLandmarkDetector {
  private:
    LiteCpuHandLandmarkDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~LiteCpuHandLandmarkDetector() {}
};
DetectorFactoryRegistration<LiteCpuHandLandmarkDetector, HandLandmarkDescriptor>
    handLandmark_lite_cpu("handlandmark_lite_cpu");

class FullCpuHandLandmarkDetector : public CpuHandLandmarkDetector {
  private:
    FullCpuHandLandmarkDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~FullCpuHandLandmarkDetector() {}
};
DetectorFactoryRegistration<FullCpuHandLandmarkDetector, HandLandmarkDescriptor>
    handLandmark_full_cpu("handlandmark_full_cpu");

} // end of namespace aif

#endif // AIF_CPU_HANDLANDMARK_DETECTOR_H
