/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_SEMANTIC_DETECTOR_H
#define AIF_CPU_SEMANTIC_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aifex/semantic/SemanticDescriptor.h>
#include <aifex/semantic/SemanticDetector.h>

namespace aif {

class CpuSemanticDetector : public SemanticDetector {
  private:
    CpuSemanticDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuSemanticDetector();
};

DetectorFactoryRegistration<CpuSemanticDetector, SemanticDescriptor>
    semantic_deeplabv3_cpu("semantic_deeplabv3_cpu");
} // end of namespace aif

#endif // AIF_CPU_SEMANTIC_DETECTOR_H
