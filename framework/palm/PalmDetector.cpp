/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/palm/PalmDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <tensorflow/lite/kernels/internal/reference/non_max_suppression.h>

namespace aif {

PalmDetector::PalmDetector(const std::string& modelPath)
    : Detector(modelPath)
{
}

PalmDetector::~PalmDetector()
{
}

std::shared_ptr<DetectorParam> PalmDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<PalmParam>();
    return param;
}

t_aif_status PalmDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv image!!");
        }

        int height = m_modelInfo.height;
        int width = m_modelInfo.width;
        int channels = m_modelInfo.channels;

        if (m_interpreter == nullptr) {
            throw std::runtime_error("palm_detection_lite.tflite interpreter not initialized!!");
        }
        t_aif_status res = aif::fillInputTensor<float, cv::Vec3f>(
                                    m_interpreter.get(),
                                    img,
                                    width,
                                    height,
                                    channels,
                                    false,
                                    aif::kAifNormalization
                                );
 
        if (res != kAifOk) {
            throw std::runtime_error("fillInputTensor failed!!");
        }

        return res;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status PalmDetector::preProcessing()
{
    try {
        Stopwatch sw;
        sw.start();
        // prepare anchors for palm detection
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
    return kAifOk;
}

void PalmDetector::printOutput() {
    const std::vector<int>& outputs = m_interpreter->outputs();
    for (int i = 0; i < outputs.size(); i++) {
        TfLiteTensor *output= m_interpreter->tensor(outputs[i]);
        TRACE(i, ":output bytes: ", output->bytes);
        TRACE(i, ":output type: ", output->type);
        TRACE(i, ":output dims 0: ", output->dims->data[0]);
        TRACE(i, ":output dims 1: ",  output->dims->data[1]);
        TRACE(i, ":output dims 2: ",  output->dims->data[2]);
        TRACE(i, ":output dims 3: ",  output->dims->data[3]);
    }
}

t_aif_status PalmDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    try{
        Stopwatch sw;
        sw.start();
        if (palmDetect(descriptor) != kAifOk) {
            throw std::runtime_error("generateOutput failed!!");
        }
        TRACE("\tpalmDetect(): ", sw.getMs(), "ms");
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

//    const std::vector<int> &outputs = m_interpreter->outputs();
//    TfLiteTensor *output = m_interpreter->tensor(outputs[0]);
//    float* keyPoints = reinterpret_cast<float*>(output->data.data);
//
//    std::shared_ptr<PalmDescriptor> palmDescriptor = std::dynamic_pointer_cast<PalmDescriptor>(descriptor);
//    float scaleX = (float)img.size().width / (float)m_modelInfo.width;
//    float scaleY = (float)img.size().height / (float)m_modelInfo.height;
//
//    TRACE("palm img size: ", img.size());
//    TRACE("palm model size: ", m_modelInfo.width, m_modelInfo.height);
//    TRACE("scale x: ", scaleX);
//    TRACE("scale y: ", scaleY);
//
//    int k = 0;
//    std::vector<cv::Point2f> points;
//    std::vector<float> scores;
//    for (int j = 0; j < 17; j++ ) {
//        float height = keyPoints[k]; // * scaleY;
//        float width = keyPoints[k+1];// * scaleX;
//        points.push_back(cv::Point2f(width, height));
//        scores.push_back(keyPoints[k+2]);
//        k = k + 3;
//    }
//    palmDescriptor->addKeyPoints(points, scores);
//    return kAifOk;
}

/*virtual */
t_aif_status PalmDetector::generateAnchors(
    int input_size_width,
    int input_size_height)
{
    try {
        std::shared_ptr<PalmParam> param = std::dynamic_pointer_cast<PalmParam>(m_param);
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
            if (opt_feature_map_height_size > 0) {
                feature_map_height = opt_feature_map_height[layer_id];
                feature_map_width = opt_feature_map_width[layer_id];
            } else {
                const int stride = strides[layer_id];
                feature_map_height = std::ceil(1.0f * input_size_height / stride);
                feature_map_width = std::ceil(1.0f * input_size_width / stride);
            }

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


float PalmDetector::calculateScale(float min_scale, float max_scale, int stride_index, int num_strides)
{
    if (num_strides == 1) {
        return (min_scale + max_scale) * 0.5f;
    } else{
        return min_scale + (max_scale - min_scale) * 1.0f * stride_index / (num_strides - 1.0f);
    }
}

t_aif_status PalmDetector::palmDetect(std::shared_ptr<Descriptor>& descriptor)
{
    try {
        std::shared_ptr<PalmDescriptor> palmDescriptor = std::dynamic_pointer_cast<PalmDescriptor>(descriptor);
        std::shared_ptr<PalmParam> param = std::dynamic_pointer_cast<PalmParam>(m_param);
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
        std::vector<float> good_palmpoint;

        int width = m_modelInfo.width;
        int height = m_modelInfo.height;

        const float score_thresh = param->scoreThreshold;
        const float iou_threshold = param->iouThreshold;
        const int max_output_size = param->maxOutputSize;

        int numAnchor= tensor_output_coord->dims->data[1];
        int numData = tensor_output_coord->dims->data[2];
        for(int i = 0; i < numAnchor; i++) {
            float score = tensor_output_score->data.f[i];
            float* coordData = tensor_output_coord->data.f;
            score = 1.0f / (1.0f + std::exp(-score)); // apply sigmoid to score
            if (score > score_thresh) {
//                std::cout << "index: " << i << " score: " << score << std::endl;
//                std::cout << "pos: " << coordData[numData * i + 0]
//                    << coordData[numData * i + 1]
//                    << coordData[numData * i + 2]
//                    << coordData[numData * i + 3]
//                    << std::endl;
                
                good_score.push_back(score);
                float x_center = coordData[numData * i + 0];
                float y_center = coordData[numData * i + 1];
                float w = coordData[numData * i + 2];
                float h = coordData[numData * i + 3];

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

                for (int j = 4; j < numData; j += 2) {
                    float dataX = (coordData[numData * i + j] + m_anchors[i].x_center * width) / width;
                    float dataY = (coordData[numData * i + j + 1] + m_anchors[i].y_center * height) / height;
                    
                    good_palmpoint.push_back(dataX);
                    good_palmpoint.push_back(dataY);
                }
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

        TRACE("Found ", num_selected_indices, " palms...");
        for(int i = 0 ; i < num_selected_indices ; i++){
            int idx = selected_indices[i];
             std::vector<float> palmPoints;
             for (int j = 0; j < 4; j++) {
                 palmPoints.push_back(good_region[4 * idx + j]);
             }
             for (int j = 0; j < 14; j++) {
                 palmPoints.push_back(good_palmpoint[14 * idx + j]);
             }
             palmDescriptor->addPalm(selected_scores[i], palmPoints);
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


} // end of namespace aif
