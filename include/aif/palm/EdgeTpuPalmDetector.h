/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_EDGETPU_PALM_DETECTOR_H
#define AIF_EDGETPU_PALM_DETECTOR_H

#include <aif/palm/PalmDetector.h>
#include <aif/palm/PalmDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <edgetpu.h>

namespace aif {

class EdgeTpuPalmDetector : public PalmDetector
{
private:
    EdgeTpuPalmDetector();

public:
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~EdgeTpuPalmDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

DetectorFactoryRegistration<EdgeTpuPalmDetector, PalmDescriptor>
palm_model_edgetpu("palm_model_edgetpu");

} // end of namespace aif

#endif // AIF_EDGETPU_PALM_DETECTOR_H
