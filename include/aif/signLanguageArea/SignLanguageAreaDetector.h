/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_SIGNLANGUAGEAREA_DETECTOR_H
#define AIF_SIGNLANGUAGEAREA_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/signLanguageArea/SignLanguageAreaDescriptor.h>
#include <aif/signLanguageArea/SignLanguageAreaParam.h>


namespace aif {

class SignLanguageAreaDetector : public Detector
{
public:
    SignLanguageAreaDetector(const std::string& modelPath);
    virtual ~SignLanguageAreaDetector();

protected:
    std::shared_ptr<DetectorParam> createParam() override;
    void setModelInfo(TfLiteTensor* inputTensor) override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override;
};

} // end of namespace aif

#endif // AIF_SIGNLANGUAGEAREA_DETECTOR_H
