/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_BODYPIX_DETECTOR_H
#define AIF_CPU_BODYPIX_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodypix/BodypixDescriptor.h>
#include <aif/bodypix/BodypixDetector.h>

namespace aif {

class CpuBodypixDetector : public BodypixDetector {
  private:
    CpuBodypixDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuBodypixDetector();

  protected:
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override;
};

DetectorFactoryRegistration<CpuBodypixDetector, BodypixDescriptor>
    bodypix_mobilenet_cpu("bodypix_mobilenet_cpu");

} // end of namespace aif

#endif // AIF_CPU_BODYPIX_DETECTOR_H
