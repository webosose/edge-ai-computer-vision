/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_TEXT_DETECTOR_H
#define AIF_TEXT_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/text/TextDescriptor.h>
#include <aif/text/TextParam.h>


namespace aif {

class TextDetector : public Detector
{
public:
    TextDetector(const std::string& modelPath, bool useNHWC = false);
    virtual ~TextDetector();

protected:
    std::shared_ptr<DetectorParam> createParam() override;
    void setModelInfo(TfLiteTensor* inputTensor) override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override;


    void convertOutputToBoxes(cv::Mat& probabilityMap, std::vector<cv::Rect>& resultRects, cv::Size imgOriginalSize);
    bool boxScoreFast(cv::Mat &binaryMap, std::vector<cv::Point> approx, float boxThresh);
    std::vector<cv::Point> unclip(const std::vector<cv::Point> &box, float unclipRatio);
    int clip(float n, float lower, float upper);

    bool m_useNHWC;
    cv::Mat m_img;
};

} // end of namespace aif

#endif // AIF_TEXT_DETECTOR_H
