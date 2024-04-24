/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_BGSEGMENT_DETECTOR_H
#define AIF_NPU_BGSEGMENT_DETECTOR_H

#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bgSegment/BgSegmentDetector.h>
#include <opencv2/opencv.hpp>

namespace aif {

class NpuBgSegmentDetector : public BgSegmentDetector {
  protected:
    NpuBgSegmentDetector(const std::string &modelName);

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~NpuBgSegmentDetector();

  protected:
    void setModelInfo(TfLiteTensor* inputTensor) override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;
    cv::Size scaleUpMask(int width, int height, uint8_t* srcData, const cv::Mat &origImg);
    cv::Size getMask(int width, int height, uint8_t* srcData);
    cv::Size smoothingMask(const cv::Mat &img, cv::Mat &transformed_map);

  private:
    struct t_aif_padding_info m_paddingInfo;
    cv::Mat m_Mask;
    cv::Mat m_finalMask;
    cv::Rect m_roiRect;
    bool m_useRoi;
    bool m_outScaleUp;
    bool m_smoothing;
    float m_th_mad4;
    cv::Mat m_prevImg;
    cv::Mat m_accumulatedMap;
    cv::InterpolationFlags m_decimFlag;
};

} // end of namespace aif

#endif // AIF_NPU_BGSEGMENT_DETECTOR_H
