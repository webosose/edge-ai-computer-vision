/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YUNET_FACE_DETECTOR_H
#define AIF_YUNET_FACE_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/face/FaceDescriptor.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

namespace aif {

class YuNetFaceDetector : public Detector {
  private:
    YuNetFaceDetector();

  public:
    enum {
        FACE_X = 0,
        FACE_Y = 1,
        FACE_W = 2,
        FACE_H = 3,
        RIGHT_EYE_X = 4,
        RIGHT_EYE_Y = 5,
        LEFT_EYE_X = 6,
        LEFT_EYE_Y = 7,
        NOSE_TIP_X = 8,
        NOSE_TIP_Y = 9,
        RIGHT_MOUSE_X = 10,
        RIGHT_MOUSE_Y = 11,
        LEFT_MOUSE_X = 12,
        LEFT_MOUSE_Y = 13,
        SCORE = 14
    };

    enum {
        DEFAULT_INPUT_WIDTH=160,
        DEFAULT_INPUT_HEIGHT=120
    };

    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~YuNetFaceDetector();

  protected:
    std::shared_ptr<DetectorParam> createParam() override;
    void setModelInfo(TfLiteTensor* inputTensor) override;
    t_aif_status preProcessing() override;
    t_aif_status
    postProcessing(const cv::Mat &img,
                   std::shared_ptr<Descriptor> &descriptor) override;
    t_aif_status fillInputTensor(const cv::Mat &img) override;

    void generatePriors();
    void getPaddedImage(const cv::Mat& src, const cv::Size& modelSize, cv::Mat& dst);
    void convertToDescriptor(cv::Mat& faces, std::shared_ptr<Descriptor>& descriptor);
    t_aif_status faceDetect(std::shared_ptr<Descriptor>& descriptor);
  private:
    std::vector<cv::Rect2f> m_priors;
    cv::Size m_scaleSize;
    cv::Size m_paddedSize;
    int m_inputWidth;
    int m_inputHeight;
};

DetectorFactoryRegistration<YuNetFaceDetector, FaceDescriptor>
    face_yunet_cpu("face_yunet_cpu");

} // end of namespace aif

#endif // AIF_YUNET_FACE_DETECTOR_H
