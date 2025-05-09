/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POSENET_DETECTOR_H
#define AIF_POSENET_DETECTOR_H

#include <aif/base/Detector.h>
#include <aifex/pose/PosenetDescriptor.h>
#include <aifex/pose/PosenetParam.h>

namespace aif {

class PosenetDetector : public Detector
{
public:
    PosenetDetector(const std::string& modelPath);
    virtual ~PosenetDetector();

    std::shared_ptr<DetectorParam> createParam() override;

protected:
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;
    
private:
    void printOutput();
    std::vector<std::vector<cv::Rect2f>> m_prevPoses;
    float m_iouThreshold;

};

} // end of namespace aif

#endif // AIF_POSENET_DETECTOR_H
