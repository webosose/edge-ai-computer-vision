/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV4_DETECTOR_H
#define AIF_YOLOV4_DETECTOR_H

#include <aif/bodyPoseEstimation/common.h>
#include <aif/bodyPoseEstimation/personDetect/PersonDetectDetector.h>
#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Param.h>


namespace aif {

class Yolov4Detector : public PersonDetector
{
public:
    Yolov4Detector(const std::string& modelPath);
    virtual ~Yolov4Detector();

protected:
    std::shared_ptr<DetectorParam> createParam() override;
    void setModelInfo(TfLiteTensor* inputTensor) override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override;

private:
    cv::Mat staticResize(const cv::Mat& img, const int targetWidth, const int targetHeight);
    t_aif_status getOutputTensorInfo(uint8_t **outTensorDataArr,
                                     size_t &outConcatDimSize);

    t_aif_status transformToImageCoordinates(uint8_t **outTensorDataArr, float *target_outTensor_deq,
                                                 std::shared_ptr<Yolov4Param> &param);
    void generateYolov4Proposals( std::vector<Object>& objects,
                                  std::shared_ptr<Yolov4Param> &param,
                                  float *outData, size_t outSize);
    void qsortDescentInplace( std::vector<Object>& objects );
    void qsortDescentInplace( std::vector<Object>& objects, int left, int right );
    void nmsSortedBboxes( std::shared_ptr<Yolov4Param> &param, const std::vector<Object>& objects, std::vector<unsigned int>& picked );
    static inline float intersectionArea( const Object& a, const Object& b )
    {
        cv::Rect_<float> inter = a.rect & b.rect;
        return inter.area();
    }

private:
    float mScaleOut[RESULT_YOLO_IDX_MAX];
    int mZeropointOut[RESULT_YOLO_IDX_MAX];

};

} // end of namespace aif

#endif // AIF_YOLOV4_DETECTOR_H
