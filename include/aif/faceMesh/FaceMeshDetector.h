/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FACEMESH_DETECTOR_H
#define AIF_FACEMESH_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/faceMesh/FaceMeshDescriptor.h>
#include <aif/faceMesh/FaceMeshParam.h>

namespace aif {

class FaceMeshDetector : public Detector
{
public:
    FaceMeshDetector(const std::string& modelPath);
    virtual ~FaceMeshDetector();

    std::shared_ptr<DetectorParam> createParam() override;

protected:
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;

private:
    void printOutput();
};

} // end of namespace aif

#endif // AIF_FACEMESH_DETECTOR_H
