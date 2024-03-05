/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3PTQDetector.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Param.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

Yolov3PTQDetector::Yolov3PTQDetector()
: Yolov3Detector("FitTV_Detector_Yolov3_PTQ.tflite", static_cast<int>(VersionType::MODEL_V2)) {}

void Yolov3PTQDetector::setModelInOutInfo(const std::vector<int> &t_inputs,
                                          const std::vector<int> &t_outputs)
{
    if (t_inputs.size() != 1) {
        Loge("input tensor size should be 1 ");
        return ;
    }

    TfLiteTensor *inputTensor = m_interpreter->tensor(t_inputs[0]);
    if (inputTensor == nullptr || inputTensor->dims == nullptr) {
        Loge("tflite inputTensor invalid!!");
        return;
    }

    if (inputTensor->dims == nullptr) {
        Loge("invalid inputTensor dimension!");
        return;
    }

    m_modelInfo.inputSize = inputTensor->dims->size;
    m_modelInfo.batchSize = inputTensor->dims->data[0]; // batchSize
    m_modelInfo.height    = inputTensor->dims->data[1]; // height
    m_modelInfo.width     = inputTensor->dims->data[2]; // width
    m_modelInfo.channels  = inputTensor->dims->data[3]; // channels

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:"     , m_modelInfo.height);
    TRACE("width: "     , m_modelInfo.width);
    TRACE("channels: "  , m_modelInfo.channels);


    if (t_outputs.size() != 6) {
        Loge("output tensor size should be 6!");
        return ;
    }

    output_sb_box = m_interpreter->tensor(t_outputs[0]); // sb_box
    output_lb_conf = m_interpreter->tensor(t_outputs[1]); // lb_conf
    output_mb_conf = m_interpreter->tensor(t_outputs[2]); // mb_conf
    output_mb_box = m_interpreter->tensor(t_outputs[3]); // mb_box
    output_sb_conf = m_interpreter->tensor(t_outputs[4]); // sb_conf
    output_lb_box = m_interpreter->tensor(t_outputs[5]); // lb_box

#if 0
    for (int i = 0; i < t_outputs.size(); i++) {
        TfLiteTensor *output = m_interpreter->tensor(t_outputs[i]);
        for (int j = 0; j < output->dims->size; j++) {
            std::cout << output->dims->data[j] << " ";
        }
        std::cout << "\n";
    }
#endif

}

void Yolov3PTQDetector::Read_Result(std::vector<unsigned short> &obd_lb_box,
                                 std::vector<unsigned short> &obd_lb_conf,
                                 std::vector<unsigned short> &obd_mb_box,
                                 std::vector<unsigned short> &obd_mb_conf,
                                 std::vector<unsigned short> &obd_sb_box,
                                 std::vector<unsigned short> &obd_sb_conf)
{
    RD_Result_box_conf(obd_lb_box,  output_lb_box->data.int8);
    RD_Result_box_conf(obd_lb_conf, output_lb_conf->data.int8);
    RD_Result_box_conf(obd_mb_box,  output_mb_box->data.int8);
    RD_Result_box_conf(obd_mb_conf, output_mb_conf->data.int8);
    RD_Result_box_conf(obd_sb_box,  output_sb_box->data.int8);
    RD_Result_box_conf(obd_sb_conf, output_sb_conf->data.int8);
}

void Yolov3PTQDetector::RD_Result_box_conf(std::vector<unsigned short>& obd_box_conf, int8_t *addr)
{
    uint8_t data;

    for (int i = 0; i < obd_box_conf.size(); i++) {
        data = *addr + 0x80;  // TODO : Think about it !!! to change DEQ
        obd_box_conf[i] = (unsigned short)data;
#if 0
        if (obd_conf.size() == SZ_CONV_LB_CONF && i < 100) {
            printf("[%d:0x%x] = 0x%x\n", i, i, obd_conf[i]);
        }
#endif
        ++addr;
    }
}


} // end of namespace aif
