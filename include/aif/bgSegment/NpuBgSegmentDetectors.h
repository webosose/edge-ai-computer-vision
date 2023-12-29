

/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_BGSEGMENT_DETECTORS_H
#define AIF_NPU_BGSEGMENT_DETECTORS_H

#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bgSegment/NpuBgSegmentDetector.h>

namespace aif {

class NpuBgSegmentDetector_V543 : public NpuBgSegmentDetector {
    private:
        NpuBgSegmentDetector_V543();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuBgSegmentDetector_V543() {}

};
DetectorFactoryRegistration<NpuBgSegmentDetector_V543, BgSegmentDescriptor>
    bg_segment_npu_v543("bg_segmentation_npu"); // <<<--- Default in 12/29.


class NpuBgSegmentDetector_V531 : public NpuBgSegmentDetector {
    private:
        NpuBgSegmentDetector_V531();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuBgSegmentDetector_V531() {}

};
DetectorFactoryRegistration<NpuBgSegmentDetector_V531, BgSegmentDescriptor>
    bg_segment_npu_v531("bg_segmentation_npu_v531");


} // end of namespace aif


#endif // AIF_NPU_BGSEGMENT_DETECTORS_H

