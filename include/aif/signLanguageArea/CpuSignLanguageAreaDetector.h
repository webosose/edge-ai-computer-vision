/* * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_SIGNLANGUAGEAREA_DETECTOR_H
#define AIF_CPU_SIGNLANGUAGEAREA_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/signLanguageArea/SignLanguageAreaDescriptor.h>
#include <aif/signLanguageArea/SignLanguageAreaDetector.h>

namespace aif {

class CpuSignLanguageAreaDetector : public SignLanguageAreaDetector {
  private:
    CpuSignLanguageAreaDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuSignLanguageAreaDetector();
};

DetectorFactoryRegistration<CpuSignLanguageAreaDetector, SignLanguageAreaDescriptor>
    signlanguagearea_model_cpu("signlanguagearea_model_cpu");

} // end of namespace aif

#endif // AIF_CPU_SIGNLANGUAGEAREA_DETECTOR_H
