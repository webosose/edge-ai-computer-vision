/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_EDGETPU_MOVENET_DETECTOR_H
#define AIF_EDGETPU_MOVENET_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/movenet/MovenetDescriptor.h>
#include <aif/movenet/MovenetDetector.h>
#include <edgetpu.h>

namespace aif {

class EdgeTpuMovenetDetector : public MovenetDetector {
  private:
    EdgeTpuMovenetDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~EdgeTpuMovenetDetector();

  protected:
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override;

  protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

DetectorFactoryRegistration<EdgeTpuMovenetDetector, MovenetDescriptor>
    movenet_thunder_edgetpu("movenet_thunder_edgetpu");

} // end of namespace aif

#endif // AIF_EDGETPU_MOVENET_DETECTOR_H
