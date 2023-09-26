/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_BGSEGMENT_DETECTOR_H
#define AIF_BGSEGMENT_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/bgSegment/BgSegmentDescriptor.h>
#include <aif/bgSegment/BgSegmentParam.h>

namespace aif {

class BgSegmentDetector : public Detector
{
public:
    BgSegmentDetector(const std::string& modelPath);
    virtual ~BgSegmentDetector();

protected:
    std::shared_ptr<DetectorParam> createParam() override;
private:
};

} // end of namespace aif

#endif // AIF_BGSEGMENT_DETECTOR_H
