/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_MNAME_DETECTOR_H
#define AIF_MNAME_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/mname/MNameDescriptor.h>
#include <aif/mname/MNameParam.h>


namespace aif {

class MNameDetector : public Detector
{
public:
    MNameDetector(const std::string& modelPath);
    virtual ~MNameDetector();

protected:
    std::shared_ptr<DetectorParam> createParam() override;
    void setModelInfo(TfLiteTensor* inputTensor) override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override;
};

} // end of namespace aif

#endif // AIF_MNAME_DETECTOR_H
