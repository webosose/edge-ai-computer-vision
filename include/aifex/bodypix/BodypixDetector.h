/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_BODYPIX_DETECTOR_H
#define AIF_BODYPIX_DETECTOR_H

#include <aif/base/Detector.h>
#include <aifex/bodypix/BodypixDescriptor.h>
#include <aifex/bodypix/BodypixParam.h>

namespace aif {

class BodypixDetector : public Detector
{
public:
    BodypixDetector( const std::string& modelPath);
    virtual ~BodypixDetector();

protected:
    std::shared_ptr<DetectorParam> createParam() override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override;
};

} // end of namespace aif

#endif // AIF_BODYPIX_DETECTOR_H
