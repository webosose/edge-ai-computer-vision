/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FACE_DETECTOR_H
#define AIF_FACE_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/face/FaceDescriptor.h>
#include <aif/face/FaceParam.h>

namespace aif {

class FaceDetector : public Detector {
  public:
    FaceDetector(const std::string &modelPath);
    virtual ~FaceDetector();

  protected:
    std::shared_ptr<DetectorParam> createParam() override;
    t_aif_status preProcessing() override;
    t_aif_status
    postProcessing(const cv::Mat &img,
                   std::shared_ptr<Descriptor> &descriptor) override;

    t_aif_status generateAnchors(int width, int height);
    t_aif_status faceDetect(std::shared_ptr<Descriptor> &descriptor);
    float calculateScale(float min_scale, float max_scale, size_t stride_index,
                         size_t num_strides);
    bool checkUpdate(size_t idx, float cur_x, float cur_y, float cur_w,
                     float cur_h, float threshold);

  protected:
    std::vector<t_aif_anchor> m_anchors;
    std::vector<std::vector<float>> m_prev_faces;
};

} // end of namespace aif

#endif // AIF_FACE_DETECTOR_H
