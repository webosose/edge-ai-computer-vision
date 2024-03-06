/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_EDGETPU_SEMANTIC_DETECTOR_H
#define AIF_EDGETPU_SEMANTIC_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aifex/semantic/SemanticDescriptor.h>
#include <aifex/semantic/SemanticDetector.h>

#include <edgetpu.h>

namespace aif {

class EdgeTpuSemanticDetector : public SemanticDetector {
  private:
    EdgeTpuSemanticDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~EdgeTpuSemanticDetector();

  protected:
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override;

  protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

DetectorFactoryRegistration<EdgeTpuSemanticDetector, SemanticDescriptor>
    semantic_deeplabv3_edgetpu("semantic_deeplabv3_edgetpu");

} // end of namespace aif

#endif // AIF_EDGETPU_SEMANTIC_DETECTOR_H
