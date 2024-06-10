// Copyright 2019 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef AIF_FACE_DETECTOR_H
#define AIF_FACE_DETECTOR_H

#include <aif/base/Detector.h>
#include <aifex/face/FaceDescriptor.h>
#include <aifex/face/FaceParam.h>

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
