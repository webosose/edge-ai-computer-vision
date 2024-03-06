/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_EDGETPU_BODYPIX_DETECTOR_H
#define AIF_EDGETPU_BODYPIX_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aifex/bodypix/BodypixDescriptor.h>
#include <aifex/bodypix/BodypixDetector.h>

#include <edgetpu.h>

namespace aif {

class EdgeTpuBodypixDetector : public BodypixDetector {
  private:
    EdgeTpuBodypixDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~EdgeTpuBodypixDetector();

  protected:
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override;

  protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

DetectorFactoryRegistration<EdgeTpuBodypixDetector, BodypixDescriptor>
    bodypix_mobilenet_edgetpu("bodypix_mobilenet_edgetpu");

} // end of namespace aif

#endif // AIF_EDGETPU_BODYPIX_DETECTOR_H
