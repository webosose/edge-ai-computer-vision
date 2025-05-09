/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2013, OpenCV Foundation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#ifndef AIF_YUNET_FACE_DETECTOR_H
#define AIF_YUNET_FACE_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aifex/face/FaceDescriptor.h>

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
