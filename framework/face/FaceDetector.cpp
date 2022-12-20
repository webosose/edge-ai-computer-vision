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

#include <aif/face/FaceDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <tensorflow/lite/kernels/internal/reference/non_max_suppression.h>

namespace {

} // end of anonymous namespace

using aif::Stopwatch;

namespace aif {

FaceDetector::FaceDetector(const std::string& modelPath)
    : Detector(modelPath)
{
}

FaceDetector::~FaceDetector()
{
}

std::shared_ptr<DetectorParam> FaceDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<FaceParam>();
    return param;
}

t_aif_status FaceDetector::preProcessing()
{
    try {
        Stopwatch sw;
        sw.start();
        // prepare anchors for face detection
        if (generateAnchors(m_modelInfo.width,
                    m_modelInfo.height) != kAifOk) {
            throw std::runtime_error("generateAnchors failed!!");
        }
        TRACE("generateAnchors(): ", sw.getMs(), "ms");
        sw.stop();
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
    return kAifOk;
}

t_aif_status FaceDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    try{
        Stopwatch sw;
        sw.start();
        if (faceDetect(descriptor) != kAifOk) {
            throw std::runtime_error("generateOutput failed!!");
        }
        TRACE("\tfaceDetect(): ", sw.getMs(), "ms");
        sw.stop();
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
    return kAifOk;
}

/*virtual */
t_aif_status FaceDetector::generateAnchors(
    int input_size_width,
    int input_size_height)
{
    try {
        std::shared_ptr<FaceParam> param = std::dynamic_pointer_cast<FaceParam>(m_param);
        // CID 9333372, CID 9333380
        if (param == nullptr) {
            throw std::runtime_error("param is null pointer!");
        }
        const std::vector<int> strides = param->strides;
        const int num_layers = static_cast<int>(strides.size());
        if (num_layers == 0) {
            throw std::runtime_error("invalid strides: num_layers is 0!");
        }

        const float interpolated_scale_aspect_ratio = param->interpolatedScaleAspectRatio;
        const float min_scale = param->minScale;
        const float max_scale = param->maxScale;
        const bool reduce_boxes_in_lowest_layer = param->reduceBoxesInLowestLayer;

        std::vector<float> opt_aspect_ratios = param->optAspectRatios;
        const int opt_aspect_ratios_size = opt_aspect_ratios.size();

        std::vector<float> opt_feature_map_height;
        std::vector<float> opt_feature_map_width;
        int opt_feature_map_height_size = 0;

        const float anchor_offset_x = param->anchorOffsetX;
        const float anchor_offset_y = param->anchorOffsetY;

        int layer_id = 0;
        while (layer_id < num_layers) {
            std::vector<float> anchor_height;
            std::vector<float> anchor_width;
            std::vector<float> aspect_ratios;
            std::vector<float> scales;

            int last_same_stride_layer = layer_id;
            while (last_same_stride_layer < num_layers &&
                param->strides[last_same_stride_layer] == strides[layer_id]) {
                const float scale = calculateScale(
                    min_scale, max_scale, last_same_stride_layer, num_layers);
                if (last_same_stride_layer == 0 && reduce_boxes_in_lowest_layer) {
                    aspect_ratios.push_back(1.0f);
                    aspect_ratios.push_back(2.0f);
                    aspect_ratios.push_back(0.5f);
                    scales.push_back(0.1f);
                    scales.push_back(scale);
                    scales.push_back(scale);
                } else {
                    for (int opt_aspect_ratio_id = 0 ;
                        opt_aspect_ratio_id < opt_aspect_ratios_size;
                        opt_aspect_ratio_id++) {
                        aspect_ratios.push_back(opt_aspect_ratios[opt_aspect_ratio_id]);
                        scales.push_back(scale);
                    }
                    if (interpolated_scale_aspect_ratio > 0.0f) {
                        const float scale_next = last_same_stride_layer == num_layers - 1 ?
                        1.0f : calculateScale(min_scale, max_scale, last_same_stride_layer +1, num_layers);
                        scales.push_back(std::sqrt(scale * scale_next));
                        aspect_ratios.push_back(interpolated_scale_aspect_ratio);
                    }
                }
                last_same_stride_layer += 1;
            }

            for(int i = 0 ; i < aspect_ratios.size() ; i++) {
                const float ratio_sqrts = std::sqrt(aspect_ratios[i]);
                anchor_height.push_back(scales[i] / ratio_sqrts);
                anchor_width.push_back(scales[i] * ratio_sqrts);
            }

            int feature_map_height = 0;
            int feature_map_width = 0;
            // CID 9333364
            const int stride = strides[layer_id];
            feature_map_height = std::ceil(1.0f * input_size_height / stride);
            feature_map_width = std::ceil(1.0f * input_size_width / stride);

            for (int y = 0; y < feature_map_height; y++) {
                for (int x = 0; x < feature_map_width; x++) {
                    for(int anchor_id = 0 ; anchor_id < anchor_height.size() ; anchor_id++) {
                        float x_center = (x + anchor_offset_x) * 1.0f / feature_map_width;
                        float y_center = (y + anchor_offset_y) * 1.0f / feature_map_height;
                        // fixed_anchor_size == true
                        t_aif_anchor new_anchor;
                        new_anchor.x_center = x_center;
                        new_anchor.y_center = y_center;
                        new_anchor.w = 1.0f;
                        new_anchor.h = 1.0f;
                        m_anchors.push_back(new_anchor);
                    }
                }
            }
            layer_id = last_same_stride_layer;
        }
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

/*virtual */
t_aif_status FaceDetector::faceDetect(std::shared_ptr<Descriptor>& descriptor)
{
    try {
        std::shared_ptr<FaceDescriptor> faceDescriptor = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
        std::shared_ptr<FaceParam> param = std::dynamic_pointer_cast<FaceParam>(m_param);
        if (m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter not initilaized!!");
        }

        const std::vector<int>& t_outputs = m_interpreter->outputs();
        TRACE("t_outputs.size: ", t_outputs.size());
        TfLiteTensor* tensor_output_coord = m_interpreter->tensor(t_outputs[0]);
        if (tensor_output_coord == nullptr) {
            throw std::runtime_error("can't get tflite tensor_output_coord!!");
        }
        TfLiteTensor *tensor_output_score = m_interpreter->tensor(t_outputs[1]);
        if (tensor_output_score == nullptr) {
            throw std::runtime_error("can't get tflite tensor_output_score!!");
        }

        std::vector<float> good_score;
        std::vector<float> good_region;
        std::vector<float> good_facepoint;

        int width = m_modelInfo.width;
        int height = m_modelInfo.height;

        const float score_thresh = param->scoreThreshold;
        const float iou_threshold = param->iouThreshold;
        const int max_output_size = param->maxOutputSize;

        for(int i = 0; i < tensor_output_coord->dims->data[1]; i++) {
            float score = tensor_output_score->data.f[i];
            score = 1.0f / (1.0f + std::exp(-score)); // apply sigmoid to score
            if (score > score_thresh) {
                good_score.push_back(score);

                float x_center = tensor_output_coord->data.f[16 * i + 0];
                float y_center = tensor_output_coord->data.f[16 * i + 1];
                float w = tensor_output_coord->data.f[16 * i + 2];
                float h = tensor_output_coord->data.f[16 * i + 3];
                float lefteye_x = tensor_output_coord->data.f[16 * i + 4];
                float lefteye_y = tensor_output_coord->data.f[16 * i + 5];
                float righteye_x = tensor_output_coord->data.f[16 * i + 6];
                float righteye_y = tensor_output_coord->data.f[16 * i + 7];
                float nosetip_x = tensor_output_coord->data.f[16 * i + 8];
                float nosetip_y = tensor_output_coord->data.f[16 * i + 9];
                float mouth_x = tensor_output_coord->data.f[16 * i + 10];
                float mouth_y = tensor_output_coord->data.f[16 * i + 11];
                float leftear_x = tensor_output_coord->data.f[16 * i + 12];
                float leftear_y = tensor_output_coord->data.f[16 * i + 13];
                float rightear_x = tensor_output_coord->data.f[16 * i + 14];
                float rightear_y = tensor_output_coord->data.f[16 * i + 15];

                x_center = x_center + m_anchors[i].x_center * width;
                y_center = y_center + m_anchors[i].y_center * height;

                x_center /= width;
                y_center /= height;
                w /= width;
                h /= height;

                float xmin = x_center - w * 0.5f;
                float ymin = y_center - h * 0.5f;
                float xmax = x_center + w * 0.5f;
                float ymax = y_center + h * 0.5f;

                good_region.push_back(xmin);
                good_region.push_back(ymin);
                good_region.push_back(xmax);
                good_region.push_back(ymax);

                lefteye_x = (lefteye_x +  m_anchors[i].x_center * width) / width;
                lefteye_y = (lefteye_y + m_anchors[i].y_center * height) / height;
                righteye_x = (righteye_x +  m_anchors[i].x_center * width) / width;
                righteye_y = (righteye_y + m_anchors[i].y_center * height) / height;
                nosetip_x = (nosetip_x +  m_anchors[i].x_center * width) / width;
                nosetip_y = (nosetip_y + m_anchors[i].y_center * height) / height;
                mouth_x = (mouth_x +  m_anchors[i].x_center * width) / width;
                mouth_y = (mouth_y + m_anchors[i].y_center * height) / height;
                leftear_x = (leftear_x +  m_anchors[i].x_center * width) / width;
                leftear_y = (leftear_y + m_anchors[i].y_center * height) / height;
                rightear_x = (rightear_x +  m_anchors[i].x_center * width) / width;
                rightear_y = (rightear_y + m_anchors[i].y_center * height) / height;

                good_facepoint.push_back(lefteye_x);
                good_facepoint.push_back(lefteye_y);
                good_facepoint.push_back(righteye_x);
                good_facepoint.push_back(righteye_y);
                good_facepoint.push_back(nosetip_x);
                good_facepoint.push_back(nosetip_y);
                good_facepoint.push_back(mouth_x);
                good_facepoint.push_back(mouth_y);
                good_facepoint.push_back(leftear_x);
                good_facepoint.push_back(leftear_y);
                good_facepoint.push_back(rightear_x);
                good_facepoint.push_back(rightear_y);
            }
        }

        int select_num = good_score.size();
        std::vector<int> selected_indices(select_num);
        std::vector<float> selected_scores(select_num);
        int num_selected_indices;

        tflite::reference_ops::NonMaxSuppression(good_region.data(), good_score.size(),
                                good_score.data(), max_output_size,
                                iou_threshold,
                                score_thresh,
                                0.0/*soft_nms_sigma*/, selected_indices.data(),
                                selected_scores.data(),
                                &num_selected_indices);

        TRACE("Found ", num_selected_indices, " faces...");
        for(int i = 0 ; i < num_selected_indices ; i++){
            int idx = selected_indices[i];
            bool doUpdate = checkUpdate(i, good_region[4*idx+0], good_region[4*idx+1],
                                        good_region[4*idx+2] - good_region[4*idx+0],
                                        good_region[4*idx+3] - good_region[4*idx+1],
                                        param->updateThreshold);
            if(doUpdate){
                m_prev_faces[i][0] = good_region[4*idx+0];
                m_prev_faces[i][1] = good_region[4*idx+1];
                m_prev_faces[i][2] = good_region[4*idx+2];
                m_prev_faces[i][3] = good_region[4*idx+3];
                m_prev_faces[i][4] = good_facepoint[12*idx+0];
                m_prev_faces[i][5] = good_facepoint[12*idx+1];
                m_prev_faces[i][6] = good_facepoint[12*idx+2];
                m_prev_faces[i][7] = good_facepoint[12*idx+3];
                m_prev_faces[i][8] = good_facepoint[12*idx+4];
                m_prev_faces[i][9] = good_facepoint[12*idx+5];
                m_prev_faces[i][10] = good_facepoint[12*idx+6];
                m_prev_faces[i][11] = good_facepoint[12*idx+7];
                m_prev_faces[i][12] = good_facepoint[12*idx+8];
                m_prev_faces[i][13] = good_facepoint[12*idx+9];
                m_prev_faces[i][14] = good_facepoint[12*idx+10];
                m_prev_faces[i][15] = good_facepoint[12*idx+11];
            }
            TRACE("face ", i, " : (",
                    m_prev_faces[i][0], ", " ,
                    m_prev_faces[i][1], ", ",
                    m_prev_faces[i][2], ", ",
                    m_prev_faces[i][3], "),  score " ,
                    selected_scores[i]);

            faceDescriptor->addFace(
                selected_scores[i],
                m_prev_faces[i][0],
                m_prev_faces[i][1],
                m_prev_faces[i][2] - m_prev_faces[i][0],
                m_prev_faces[i][3] - m_prev_faces[i][1],
                m_prev_faces[i][4],  // lefteye_x
                m_prev_faces[i][5],  // lefteye_y
                m_prev_faces[i][6],  // righteye_x
                m_prev_faces[i][7],  // righteye_y
                m_prev_faces[i][8],  // nosetip_x
                m_prev_faces[i][9],  // nosetip_y
                m_prev_faces[i][10],  // mouth_x
                m_prev_faces[i][11],  // mouth_y
                m_prev_faces[i][12],  // leftear_x
                m_prev_faces[i][13],  // leftear_y
                m_prev_faces[i][14],  // rightear_x
                m_prev_faces[i][15]   // rightear_y
            );

        }
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

float FaceDetector::calculateScale(float min_scale, float max_scale, int stride_index, int num_strides)
{
    if (num_strides == 1) {
        return (min_scale + max_scale) * 0.5f;
    } else{
        return min_scale + (max_scale - min_scale) * 1.0f * stride_index / (num_strides - 1.0f);
    }
}

bool FaceDetector::checkUpdate(int idx, float cur_x, float cur_y, float cur_w, float cur_h, float updateThreshold){
    if (m_prev_faces.size() <= idx) {
            std::vector<float> init_coord = {0, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            m_prev_faces.push_back(init_coord);
            return true;
        }

        float A = cur_w * cur_h;
        float B = (m_prev_faces[idx][2] - m_prev_faces[idx][0]) * (m_prev_faces[idx][3] - m_prev_faces[idx][1]);
        float x1, y1, w1, h1, x2, y2, w2, h2;
        if (cur_x < m_prev_faces[idx][0]) {
            x1 = cur_x;
            w1 = cur_w;
            x2 = m_prev_faces[idx][0];
            w2 = (m_prev_faces[idx][2] - m_prev_faces[idx][0]);
        } else {
            x2 = cur_x;
            w2 = cur_w;
            x1 = m_prev_faces[idx][0];
            w1 = (m_prev_faces[idx][2] - m_prev_faces[idx][0]);
        }

        if (cur_y < m_prev_faces[idx][1]) {
            y1 = cur_y;
            h1 = cur_h;
            y2 = m_prev_faces[idx][1];
            h2 = (m_prev_faces[idx][3] - m_prev_faces[idx][1]);
        } else {
            y2 = cur_y;
            h2 = cur_h;
            y1 = m_prev_faces[idx][1];
            h1 = (m_prev_faces[idx][3] - m_prev_faces[idx][1]);
        }

        float A_and_B =
            (std::min(x1 + w1, x2 + w2) - x2) *
            (std::min(y1 + h1, y2 + h2) - y2);
        float A_or_B = A + B - A_and_B;
        return (A_and_B / A_or_B) < (1.0f - updateThreshold) ? true : false;
}

} // end of namespace aif
