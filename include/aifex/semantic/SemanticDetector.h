/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_SEMANTIC_DETECTOR_H
#define AIF_SEMANTIC_DETECTOR_H

#include <aif/base/Detector.h>
#include <aifex/semantic/SemanticDescriptor.h>
#include <aifex/semantic/SemanticParam.h>

namespace aif {

class SemanticDetector : public Detector
{
public:
    SemanticDetector(const std::string& modelPath);
    virtual ~SemanticDetector();

protected:
    std::shared_ptr<DetectorParam> createParam() override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;

private:
    void printOutput();
};

} // end of namespace aif

#endif // AIF_SEMANTIC_DETECTOR_H
