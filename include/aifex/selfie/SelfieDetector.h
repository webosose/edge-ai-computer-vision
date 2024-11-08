/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_SELFIE_DETECTOR_H
#define AIF_SELFIE_DETECTOR_H

#include <aif/base/Detector.h>
#include <aifex/selfie/SelfieDescriptor.h>
#include <aifex/selfie/SelfieParam.h>

namespace aif {

class SelfieDetector : public Detector
{
public:
    SelfieDetector(const std::string& modelPath);
    virtual ~SelfieDetector();

protected:
    std::shared_ptr<DetectorParam> createParam() override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;

private:
    void printOutput();
};

} // end of namespace aif

#endif // AIF_SELFIE_DETECTOR_H
