/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_FACE_DETECTOR_H
#define AIF_CPU_FACE_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aifex/face/FaceDescriptor.h>
#include <aifex/face/FaceDetector.h>

namespace aif {

//------------------------------------------------------
// CpuFaceDetector
//------------------------------------------------------
class CpuFaceDetector : public FaceDetector {
  protected:
    CpuFaceDetector(const std::string &modelPath);
    virtual ~CpuFaceDetector();

  protected:
    t_aif_status fillInputTensor(const cv::Mat &img) override;
};

//------------------------------------------------------
// ShortRangeFaceDetector
//------------------------------------------------------
class ShortRangeFaceDetector : public CpuFaceDetector {
  private:
    ShortRangeFaceDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~ShortRangeFaceDetector();

  protected:
    std::shared_ptr<DetectorParam> createParam() override;
};

DetectorFactoryRegistration<ShortRangeFaceDetector, FaceDescriptor>
    face_short_range_cpu("face_short_range_cpu");

//------------------------------------------------------
// FullRangeFaceDetector
//------------------------------------------------------
class FullRangeFaceDetector : public CpuFaceDetector {
  private:
    FullRangeFaceDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~FullRangeFaceDetector();

  protected:
    std::shared_ptr<DetectorParam> createParam() override;
};

DetectorFactoryRegistration<FullRangeFaceDetector, FaceDescriptor>
    face_full_range_cpu("face_full_range_cpu");

} // end of namespace aif

#endif // AIF_CPU_FACE_DETECTOR_H
