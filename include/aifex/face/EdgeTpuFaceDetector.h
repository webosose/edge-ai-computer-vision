/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_EDGE_FACE_DETECTOR_H
#define AIF_EDGE_FACE_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aifex/face/FaceDescriptor.h>
#include <aifex/face/FaceDetector.h>
#include <edgetpu.h>

namespace aif {

//------------------------------------------------------
// EdgeTpuFaceDetector
//------------------------------------------------------
class EdgeTpuFaceDetector : public FaceDetector {
  public:
    EdgeTpuFaceDetector(const std::string &modelPath);

    virtual ~EdgeTpuFaceDetector();

  protected:
    t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override;

    t_aif_status fillInputTensor(const cv::Mat &img) override;

  protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

//------------------------------------------------------
// EdgeTpuShortRangeFaceDetector
//------------------------------------------------------
class EdgeTpuShortRangeFaceDetector : public EdgeTpuFaceDetector {
  public:
    EdgeTpuShortRangeFaceDetector();

    virtual ~EdgeTpuShortRangeFaceDetector();

  protected:
    std::shared_ptr<DetectorParam> createParam() override;
};

DetectorFactoryRegistration<EdgeTpuShortRangeFaceDetector, FaceDescriptor>
    face_detect_edgetpu("face_short_range_edgetpu");

} // end of namespace aif

#endif // AIF_EDGE_FACE_DETECTOR_H
