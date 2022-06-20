/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_MOVENET_DETECTOR_H
#define AIF_CPU_MOVENET_DETECTOR_H

#include <aif/movenet/MovenetDetector.h>
#include <aif/movenet/MovenetDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>

namespace aif {

class CpuMovenetDetector : public MovenetDetector
{
private:
    CpuMovenetDetector();

public:    
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~CpuMovenetDetector();

protected:
    t_aif_status compileModel() override;

};

DetectorFactoryRegistration<CpuMovenetDetector, MovenetDescriptor>
movenet_thunder_cpu("movenet_thunder_cpu");

} // end of namespace aif

#endif // AIF_CPU_MOVENET_DETECTOR_H
