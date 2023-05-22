/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_FACEMESH_DETECTOR_H
#define AIF_CPU_FACEMESH_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/faceMesh/FaceMeshDescriptor.h>
#include <aif/faceMesh/FaceMeshDetector.h>

namespace aif {

class CpuFaceMeshDetector : public FaceMeshDetector {
  private:
    CpuFaceMeshDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuFaceMeshDetector();

  protected:
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override;
};

DetectorFactoryRegistration<CpuFaceMeshDetector, FaceMeshDescriptor>
    facemesh_cpu("facemesh_cpu");

} // end of namespace aif

#endif // AIF_CPU_FACEMESH_DETECTOR_H
