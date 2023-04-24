#include <aif/bodyPoseEstimation/personDetect/yolov3_v2/Yolov3V2Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <cstdio>
#include <boost/algorithm/string/predicate.hpp>

namespace aif {

constexpr int Yolov3V2Detector::tcnt_init[];

Yolov3V2Detector::Yolov3V2Detector(const std::string& modelPath)
    : PersonDetector(modelPath)
    , m_obd_result{0,}
    , m_bodyResult{0,}
    , m_ImgResizingScale(1.f)
    , m_leftBorder(0)
    , m_topBorder(0)
    , m_IsBodyDetect(true)
    , m_frameId(0)
{
}

Yolov3V2Detector::~Yolov3V2Detector()
{
}

std::shared_ptr<DetectorParam> Yolov3V2Detector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<Yolov3V2Param>();
    return param;
}

void Yolov3V2Detector::setModelInfo(TfLiteTensor* inputTensor)
{
    m_modelInfo.batchSize = 1;//inputTensor->dims->data[0];
    m_modelInfo.height = inputTensor->dims->data[0];
    m_modelInfo.width = inputTensor->dims->data[1];
    m_modelInfo.channels = inputTensor->dims->data[2];

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);
}

t_aif_status Yolov3V2Detector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
              throw std::runtime_error("invalid opencv image!!");
        }

        int height = m_modelInfo.height;
        int width = m_modelInfo.width;
        int channels = m_modelInfo.channels;

        if (m_interpreter == nullptr) {
            throw std::runtime_error("yolov4.tflite interpreter not initialized!!");
        }

        cv::Mat img_resized;
        if ( isRoiValid(img.cols, img.rows) ) {
            cv::Mat roi_img = img( cv::Rect(mOrigImgRoiX, mOrigImgRoiY, mOrigImgRoiWidth, mOrigImgRoiHeight) );
            getPaddedImage(roi_img, img_resized);
        } else {
            getPaddedImage(img, img_resized);
        }

        TRACE("resized size: ", img_resized.size());
        if (img_resized.rows != height || img_resized.cols != width) {
            throw std::runtime_error("image resize failed!!");
        }

        //img_resized.convertTo(img_resized, CV_8UC3); not needed

        uint8_t* inputTensor = m_interpreter->typed_input_tensor<uint8_t>(0);
        std::memcpy(inputTensor, img_resized.ptr<uint8_t>(0), width * height * channels * sizeof(uint8_t));

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

t_aif_status Yolov3V2Detector::preProcessing()
{
    try {
        std::shared_ptr<Yolov3V2Param> param = std::dynamic_pointer_cast<Yolov3V2Param>(m_param);

        if (boost::iequals(param->detectObject, "body")) {
            m_IsBodyDetect = true;
        } else {
            throw std::runtime_error("invalid param, not body");
        }

        mOrigImgRoiX = param->origImgRoiX;
        mOrigImgRoiY = param->origImgRoiY;
        mOrigImgRoiWidth = param->origImgRoiWidth;
        mOrigImgRoiHeight = param->origImgRoiHeight;

        for (int i = 0; i < param->gt_bboxes.size(); i++) {
            auto gt_bbox = param->gt_bboxes[i];
            BBox box;
            box.addTlhw(gt_bbox.second[0], gt_bbox.second[1], gt_bbox.second[2], gt_bbox.second[3]);
            m_GTBBoxes.push_back(std::make_pair(gt_bbox.first, box));
        }

        Logi(__func__, " m_GTBBoxes.size: ", m_GTBBoxes.size());

        return kAifOk;
    } catch (const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
    return kAifOk;
}

t_aif_status Yolov3V2Detector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    try {
        Stopwatch sw;
        sw.start();
        const std::vector<int> &outputs = m_interpreter->outputs();
        TfLiteTensor *output_4 = m_interpreter->tensor(outputs[0]); // mb_conf
        TfLiteTensor *output_3 = m_interpreter->tensor(outputs[1]); // mb_box
        TfLiteTensor *output_2 = m_interpreter->tensor(outputs[2]); // lb_conf
        TfLiteTensor *output_1 = m_interpreter->tensor(outputs[3]); // lb_box
        t_pqe_obd_result *result = nullptr;

        result = &m_bodyResult; // if (m_IsBodyDetect)

        unsigned short obd_lb_box [SZ_CONV_LB_BOX ];	// lb box coordinate
        unsigned short obd_lb_conf[SZ_CONV_LB_CONF];	// lb confidence
        unsigned short obd_mb_box [SZ_CONV_MB_BOX ];	// mb box coordinate
        unsigned short obd_mb_conf[SZ_CONV_MB_CONF];	// mb confidence

        // De-Quantization + Quantization
        RD_Result_box (obd_lb_box, reinterpret_cast<unsigned int*>(output_1->data.data), SZ_LBBOX_W, SZ_LBBOX_H);
        RD_Result_conf (obd_lb_conf, reinterpret_cast<unsigned int*>(output_2->data.data), SZ_LBBOX_W, SZ_LBBOX_H);
        RD_Result_box (obd_mb_box, reinterpret_cast<unsigned int*>(output_3->data.data), SZ_MBBOX_W, SZ_MBBOX_H);
        RD_Result_conf(obd_mb_conf, reinterpret_cast<unsigned int*>(output_4->data.data), SZ_MBBOX_W, SZ_MBBOX_H);

        OBD_ComputeResult(obd_lb_box, obd_lb_conf, obd_mb_box, obd_mb_conf);
        transform2OriginCoord(img, *result);

        std::shared_ptr<Yolov3V2Param> param = std::dynamic_pointer_cast<Yolov3V2Param>(m_param);
        std::shared_ptr<Yolov3V2Descriptor> yolov3Descriptor = std::dynamic_pointer_cast<Yolov3V2Descriptor>(descriptor);

        std::vector<std::pair<BBox, float>> finalBboxList;
        for (int i=0; i< result->res_cnt; i++) {
            BBox finalBbox( img.cols , img.rows );
            finalBbox.addXyxy(result->pos_x0[i], result->pos_y0[i],
                         result->pos_x1[i], result->pos_y1[i], false);
            finalBboxList.emplace_back(finalBbox, result->res_val[i]);
        }

        // sort boxes with area descending order
        std::sort(finalBboxList.begin(), finalBboxList.end(),
                  [](const std::pair<BBox,float> &a, const std::pair<BBox,float> &b) {
                      return (a.first.width * a.first.height) > (b.first.width * b.first.height);
                  } );

        for (int i=0; (i<finalBboxList.size()) && (i < param->numMaxPerson); i++) {
            if (!checkUpdate(i, finalBboxList[i].first)) {
                finalBboxList[i].second = m_prevBboxList[i].second; // score
                finalBboxList[i].first = m_prevBboxList[i].first;   // bbox
            }

            float score = (finalBboxList[i].second / OBD_SCORE_MAX);
            const BBox &finalBbox = finalBboxList[i].first;


            if (m_GTBBoxes.size() > m_frameId) {
                auto gtBBox = m_GTBBoxes[m_frameId]; // pair
                if (gtBBox.second.width > 0 && gtBBox.second.height > 0) {
                    yolov3Descriptor->addPerson(score, gtBBox.second);
                    Logi(__func__, " use GT Box !! ", gtBBox.first, ", ", gtBBox.second);
                }
            } else {
                yolov3Descriptor->addPerson(score, finalBbox);
            }

        }
        m_frameId++;
        m_prevBboxList = finalBboxList;
        TRACE("postProcessing(): ", sw.getMs(), "ms");
        sw.stop();
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }

    return kAifOk;
}

void Yolov3V2Detector::getPaddedImage(const cv::Mat& src, cv::Mat& dst)
{
    float srcW = src.size().width;
    float srcH = src.size().height;
    int dstW = 0;
    int dstH = 0;

    // r : 0.5625 ( 270/480 < 480/640)
    // r : 0.28125 ( 270/960 vs. 480/1280)
    m_ImgResizingScale = std::min( m_modelInfo.width / (srcW * 1.0), m_modelInfo.height / (srcH * 1.0) );
    TRACE(__func__, " m_ImgResizingScale = " , m_ImgResizingScale);

    int width = srcW * m_ImgResizingScale;
    int height = srcH * m_ImgResizingScale;
    // 360 x 270
    // 360 x 270

    cv::Mat inputImg;
    cv::resize(src, inputImg, cv::Size(width, height), 0, 0, cv::INTER_LINEAR); // AREA

    m_leftBorder = 0;
    m_topBorder = 0;
    if (m_modelInfo.width != width)
        m_leftBorder = (m_modelInfo.width - width) / 2;
    if (m_modelInfo.height != height)
        m_topBorder = (m_modelInfo.height - height) / 2;

    int rightBorder = m_modelInfo.width - width - m_leftBorder;
    int bottomBorder = m_modelInfo.height - height - m_topBorder;

    TRACE(__func__, "lb, tb, rb, bb: ", m_leftBorder, ", ", m_topBorder, ", ", rightBorder, " , ", bottomBorder);

    cv::copyMakeBorder(inputImg, dst, m_topBorder, bottomBorder, m_leftBorder, rightBorder, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    m_paddedSize = cv::Size(
            srcW * (static_cast<float>(m_modelInfo.width)/width),
            srcH * (static_cast<float>(m_modelInfo.height)/height));
}


void Yolov3V2Detector::transform2OriginCoord(const cv::Mat& img, t_pqe_obd_result &result)
{
    int img_width = img.cols;
    int img_height = img.rows;
    auto off_x = 0.0f;
    auto off_y = 0.0f;
    auto roi_img_width = img_width;
    auto roi_img_height = img_height;

    if (isRoiValid(img_width, img_height)) {
        off_x = static_cast<float>( mOrigImgRoiX );
        off_y = static_cast<float>( mOrigImgRoiY );
        roi_img_width = static_cast<float>( mOrigImgRoiWidth );
        roi_img_height = static_cast<float>( mOrigImgRoiHeight );
    }

    if (m_ImgResizingScale == 1.f) {
        m_ImgResizingScale = std::min( m_modelInfo.width / (roi_img_width * 1.0), m_modelInfo.height / (roi_img_height * 1.0) );
    }

    TRACE(__func__, "m_ImgResizingScale = " , m_ImgResizingScale);
    // scale : 0.5625 ( 270/480 < 480/640)

    TRACE("---------------BEFORE------------");
    TRACE("[BODY_Result] res_cnt = ", m_bodyResult.res_cnt);
    TRACE("---------------------------");

    for(int idx = 0; idx < result.res_cnt; idx++)
    {
        TRACE(" res_val[", idx, "] = ", result.res_val[idx]);
        TRACE(" res_cls[", idx, "] = ", result.res_cls[idx]);
        TRACE(" tpr_cnt[", idx, "] = ", result.tpr_cnt[idx]);
        TRACE(" pos_x0[", idx, "] = ", result.pos_x0[idx]);
        TRACE(" pos_x1[", idx, "] = ", result.pos_x1[idx]);
        TRACE(" pos_y0[", idx, "] = ", result.pos_y0[idx]);
        TRACE(" pos_y1[", idx, "] = ", result.pos_y1[idx]);
        TRACE("---------------------------");
    }

    for (int i = 0; i < result.res_cnt; i++) {
        int x0 = ( (result.pos_x0[i] - m_leftBorder )/ m_ImgResizingScale ) + off_x;
        int y0 = ( (result.pos_y0[i] - m_topBorder ) / m_ImgResizingScale ) + off_y;
        int x1 = ( (result.pos_x1[i] - m_leftBorder) / m_ImgResizingScale ) + off_x;
        int y1 = ( (result.pos_y1[i] - m_topBorder )/ m_ImgResizingScale ) + off_y;

        result.pos_x0[i] = std::max( std::min( x0, img_width - 1 ), 0 );
        result.pos_y0[i] = std::max( std::min( y0, img_height - 1 ), 0 );
        result.pos_x1[i] = std::max( std::min( x1, img_width - 1 ), 0 );
        result.pos_y1[i] = std::max( std::min( y1, img_height - 1 ), 0 );
    }

    TRACE("-------------AFTER--------------");
    TRACE("[BODY_Result] res_cnt = ", m_bodyResult.res_cnt);
    TRACE("---------------------------");

    for(int idx = 0; idx < result.res_cnt; idx++)
    {
        TRACE(" res_val[", idx, "] = ", result.res_val[idx]);
        TRACE(" res_cls[", idx, "] = ", result.res_cls[idx]);
        TRACE(" tpr_cnt[", idx, "] = ", result.tpr_cnt[idx]);
        TRACE(" pos_x0[", idx, "] = ", result.pos_x0[idx]);
        TRACE(" pos_x1[", idx, "] = ", result.pos_x1[idx]);
        TRACE(" pos_y0[", idx, "] = ", result.pos_y0[idx]);
        TRACE(" pos_y1[", idx, "] = ", result.pos_y1[idx]);
        TRACE("---------------------------");
    }
}

void Yolov3V2Detector::OBD_ComputeResult(unsigned short* obd_lb_box_addr, unsigned short* obd_lb_conf_addr, unsigned short* obd_mb_box_addr, unsigned short* obd_mb_conf_addr)
{
    std::vector<BBox> bbox;
    CV_BOX(BoxType::LB_BOX, obd_lb_box_addr, obd_lb_conf_addr, bbox);
    CV_BOX(BoxType::MB_BOX, obd_mb_box_addr, obd_mb_conf_addr, bbox);
    TRACE(" num_bbox [", bbox.size(), "] : MAX_BOX[", MAX_BOX, "]");

    // NMS
    int num_nms = 0;
    std::vector<int> res_nms;
    if(bbox.size() <= MAX_BOX) {
        res_nms = fw_nms(bbox);
        num_nms = GET_CLIP(res_nms.size(), MAX_BOX);
    }

    Classify_OBD_Result(bbox, num_nms, res_nms);
}

void Yolov3V2Detector::CV_BOX(BoxType boxType, unsigned short *obd_addr, unsigned short *obd_conf, std::vector<BBox> &bbox)
{
    std::shared_ptr<Yolov3V2Param> param = std::dynamic_pointer_cast<Yolov3V2Param>(m_param);

    int j,i,c,cnt,cnt_box,cnt_conf,conf_jump;
    int BX0,BY0,BX1,BY1,CO;
    int BX_partial, BY_partial;
    int box_h, box_w, partial_shift, boxIdx;
    int numBox_before = bbox.size();

    if (boxType == BoxType::LB_BOX) { // for CV_LB_BOX
        box_h = param->lbbox_h;
        box_w = param->lbbox_w;
        partial_shift = 5;
        boxIdx = 0;
        conf_jump = 0;
    } else if (boxType == BoxType::MB_BOX) { // for CV_MB_BOX
        box_h = param->mbbox_h;
        box_w = param->mbbox_w;
        partial_shift = 4;
        boxIdx = 1;
        conf_jump = 4;
    } else {
        throw std::runtime_error("invalid boxType ");
    }

    cnt_box  = 0;
    cnt_conf = 0;
    for (j = 0; j < box_h; j++) {
        BY_partial = j << partial_shift;
        for (i = 0; i < box_w; i++) {
            BX_partial = i << partial_shift;
            for (c = 0; c < param->box_size; c++) {
                BX0 = cal_pos(BX_partial, LUT_EXP[boxIdx], obd_addr[cnt_box], 0, param->stride[boxIdx]);
                BY0 = cal_pos(BY_partial, LUT_EXP[boxIdx], obd_addr[cnt_box+1], 0, param->stride[boxIdx]);
                BX1 = cal_pos(BX_partial, LUT_EXP[boxIdx], obd_addr[cnt_box+2], 1, param->stride[boxIdx]);
                BY1 = cal_pos(BY_partial, LUT_EXP[boxIdx], obd_addr[cnt_box+3], 1, param->stride[boxIdx]);

                BX0 = (BX0 > m_modelInfo.width) ? m_modelInfo.width : BX0; // 480
                BY0 = (BY0 > m_modelInfo.height) ? m_modelInfo.height : BY0; // 270
                BX1 = (BX1 > m_modelInfo.width) ? m_modelInfo.width : BX1;
                BY1 = (BY1 > m_modelInfo.height) ? m_modelInfo.height : BY1;

                unsigned short max = 0;
                int max_id = -1;
                for (int n = 0; n < param->class_size; n++) {
                    if (obd_conf[cnt_conf+1+n] > max) {
                        max = obd_conf[cnt_conf+1+n];
                        max_id = n;
                    }
                }

                CO = cal_confidence(LUT_CONF[boxIdx], obd_conf[cnt_conf+conf_jump], max);
                if( CO > param->thresh_score[1]) /* low threshold */ {
                    bbox.emplace_back(BX0, BY0, BX1, BY1, CO, max_id);
                    // Logi(__func__, " bbox: ", BX0, " " ,BY0," ", BX1," ", BY1, " ",CO, " ", max_id);
                    // printf("%1d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n", boxIdx, BX0, BY0, BX1, BY1, max_id, CO);
                }
                cnt_box  += 4;
                cnt_conf += 11;
            }
        }
    }
    TRACE(__FUNCTION__, ": " , bbox.size() - numBox_before);
}

std::vector<int> Yolov3V2Detector::fw_nms(std::vector<BBox> &bbox)
{
    std::shared_ptr<Yolov3V2Param> param = std::dynamic_pointer_cast<Yolov3V2Param>(m_param);
    std::vector<int> res_nms;
    // sort
    int num_sort = 0;
    int res_sort[MAX_BOX];

    for (int idx = 0; idx < bbox.size(); idx++) {
        int c_idx = idx;
        int c_score = bbox[c_idx].c0;

        for (int idx_sort = 0; idx_sort < num_sort; idx_sort++) {
            int t_idx = res_sort[idx_sort];
            int t_score = bbox[t_idx].c0;

            if (c_score > t_score) {	// swap
                res_sort[idx_sort] = c_idx;
                c_idx = t_idx;
                c_score = t_score;
            }
        }
        res_sort[num_sort++] = c_idx;
    }

    // area
    std::vector<long int> res_area;
    for (const auto &box : bbox) {
        res_area.push_back(std::max(box.xmax - box.xmin, 0.f) * std::max(box.ymax - box.ymin, 0.f));
    }

    std::vector<int> keep_flag(bbox.size(), 1);

    for (int idx_sort = 0; idx_sort < num_sort; idx_sort++) {
        int c_idx = res_sort[idx_sort];
        long int c_area = res_area[c_idx];
        int c_score = bbox[c_idx].c0;
        int c_class = bbox[c_idx].c1;
        int th_score_avg = (c_score * 4) >> 3;
        int c_keep = keep_flag[c_idx];
        if(c_keep == 0) continue;

        for (int idx_nms = idx_sort+1; idx_nms < num_sort; idx_nms++) {
            int t_idx = res_sort[idx_nms];
            long int t_area = res_area[t_idx];
            int t_score = bbox[t_idx].c0;
            int t_class = bbox[t_idx].c1;
            int t_keep = keep_flag[t_idx];
            if(t_keep == 0) continue;

            if (c_class == t_class) {
                // intersection over union (IoU)
                int i_x0 = std::max(bbox[c_idx].xmin, bbox[t_idx].xmin);
                int i_y0 = std::max(bbox[c_idx].ymin, bbox[t_idx].ymin);
                int i_x1 = std::min(bbox[c_idx].xmax, bbox[t_idx].xmax);
                int i_y1 = std::min(bbox[c_idx].ymax, bbox[t_idx].ymax);
                long int i_area = std::max(i_x1 - i_x0, 0) * std::max(i_y1 - i_y0, 0);
                long int u_area = c_area + t_area - i_area;
                long int s_area = (c_area < t_area) ? c_area : t_area; // min value
                long int th_area_nms = (u_area * param->thresh_iou_sc_nms) >> 8;
                long int th_area_sur = (s_area * param->thresh_iou_sc_sur) >> 8;
                if (i_area > th_area_nms) keep_flag[t_idx] = 0;
                if (i_area > th_area_sur) keep_flag[t_idx] = 0;
            }
        }
        if (c_keep) {
            res_nms.push_back(c_idx);
        }
    }
    TRACE(__FUNCTION__, ": " , res_nms.size());
    return res_nms;
}

void Yolov3V2Detector::Classify_OBD_Result(const std::vector<BBox> &bbox, int num_nms, std::vector<int> &res_nms)
{
    m_obd_result = {0,};
    m_bodyResult = {0,};

    int idx = 0;
    int selState = 0;
    for (idx = 0; idx < num_nms; idx++) {
        int idx_nms = res_nms[idx];
        int _x0 = bbox[idx_nms].xmin;
        int _y0 = bbox[idx_nms].ymin;
        int _x1 = bbox[idx_nms].xmax;
        int _y1 = bbox[idx_nms].ymax;
        int _score = (bbox[idx_nms].c0);
        int _class = (bbox[idx_nms].c1);
        // printf("%2d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n", idx, _x0, _y0, _x1, _y1, _class, _score);

        selState = ((_class == eOBD_PERSON)) ? 0 : 1; // objects execept for person

        if(selState != 0 && m_obd_result.res_cnt < OBD_RESULT_NUM) {
            /* other objects except for person */
            m_obd_result.res_cls[m_obd_result.res_cnt]	= _class;
            m_obd_result.pos_x0[m_obd_result.res_cnt] 	= _x0;
            m_obd_result.pos_x1[m_obd_result.res_cnt] 	= _x1;
            m_obd_result.pos_y0[m_obd_result.res_cnt] 	= _y0;
            m_obd_result.pos_y1[m_obd_result.res_cnt] 	= _y1;

            if(_x0 != _x1 && _y0 != _y1) {
                m_obd_result.res_val[m_obd_result.res_cnt] = _score;
                m_obd_result.tpr_cnt[m_obd_result.res_cnt] = tcnt_init[_class];
                m_obd_result.res_cnt++;
            }
            // printf("%u:%2d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n", m_obd_result.res_cnt, idx, _x0, _x1, _y0, _y1, _class, _score);
            if (m_obd_result.res_cnt >= OBD_RESULT_NUM) break;
        }
        // else //only for person
        {
            if (_class == eOBD_PERSON && m_bodyResult.res_cnt < OBD_RESULT_NUM) {
                m_bodyResult.res_cls[m_bodyResult.res_cnt]	= _class;
                m_bodyResult.pos_x0[m_bodyResult.res_cnt] 	= _x0;
                m_bodyResult.pos_x1[m_bodyResult.res_cnt] 	= _x1;
                m_bodyResult.pos_y0[m_bodyResult.res_cnt] 	= _y0;
                m_bodyResult.pos_y1[m_bodyResult.res_cnt] 	= _y1;

                if(_x0 != _x1 && _y0 != _y1) {
                    m_bodyResult.res_val[m_bodyResult.res_cnt] = _score;
                    m_bodyResult.tpr_cnt[m_bodyResult.res_cnt] = tcnt_init[_class];
                    m_bodyResult.res_cnt++;
                }
                // printf("person%u:%2d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n",m_bodyResult.res_cnt, idx, _x0, _x1, _y0, _y1, _class, _score);
            }

            if(m_bodyResult.res_cnt >= OBD_RESULT_NUM) break;
        }
    }

    if(m_obd_result.res_cnt < 0 || m_obd_result.res_cnt > OBD_RESULT_NUM) m_obd_result.res_cnt = 0;
    if(m_bodyResult.res_cnt < 0 || m_bodyResult.res_cnt > OBD_RESULT_NUM) m_bodyResult.res_cnt = 0;
}

bool Yolov3V2Detector::checkUpdate(int index, const BBox &currBbox)
{
    std::shared_ptr<Yolov3V2Param> param = std::dynamic_pointer_cast<Yolov3V2Param>(m_param);
    if (m_prevBboxList.size() == 0) {
        Logd("UPDATE!!!!!!!!!!!!!!!!!");
        return true;
    }
    Logd("m_prevBboxList.size() ", m_prevBboxList.size());

    const BBox &prevBbox = m_prevBboxList[index].first;
    cv::Rect2f prevRect(prevBbox.xmin, prevBbox.ymin, prevBbox.width, prevBbox.height);
    cv::Rect2f curRect(currBbox.xmin, currBbox.ymin, currBbox.width, currBbox.height);

    if (index < m_prevBboxList.size() && isIOUOver(prevRect, curRect, param->thresh_iou_update)) {
        return false;
    }
    Logd("UPDATE!!!!!!!!!!!!!!!!!");
    return true;
}

void Yolov3V2Detector::RD_Result_box(unsigned short *obd_box, unsigned int* addr,unsigned int size_x, unsigned int size_y)
{
    int ix,iy,ic,i;
    unsigned int data;

    i = 0;
    for(iy = 0; iy < size_y; iy++) {
        for(ix = 0; ix < size_x; ix++) {
            // 16 ch = 12 (valid) + 4 (dummy)
            for(ic = 0 ; ic < 12; ic+=4) {
                data = *addr;
#if 1
                obd_box[i  ] = (data>> 0)&0xFF;
                obd_box[i+1] = (data>> 8)&0xFF;
                obd_box[i+2] = (data>>16)&0xFF;
                obd_box[i+3] = (data>>24)&0xFF;
#else
                obd_box[i  ] = (data>>24)&0xFF;
                obd_box[i+1] = (data>>16)&0xFF;
                obd_box[i+2] = (data>> 8)&0xFF;
                obd_box[i+3] = (data    )&0xFF;
#endif
                i += 4;
               addr++;
            }
            addr ++;
        }
    }
}

void Yolov3V2Detector::RD_Result_conf(unsigned short* obd_conf, unsigned int* addr,unsigned int size_x, unsigned int size_y)
{
    int ix,iy,ic,i;
    unsigned int data;

    i = 0;
    for(iy = 0; iy < size_y; iy++) {
        for(ix = 0; ix < size_x; ix++) {
            // 48 ch = 32+1 (valid) + 15 (dummy)
            for(ic = 0 ; ic < 32; ic+=4) {
                data = *addr;
#if 1
                obd_conf[i  ] = (data>> 0)&0xFF;
                obd_conf[i+1] = (data>> 8)&0xFF;
                obd_conf[i+2] = (data>>16)&0xFF;
                obd_conf[i+3] = (data>>24)&0xFF;
#else
                obd_conf[i  ] = (data>>24)&0xFF;
                obd_conf[i+1] = (data>>16)&0xFF;
                obd_conf[i+2] = (data>> 8)&0xFF;
                obd_conf[i+3] = (data    )&0xFF;
#endif
                i += 4;
               addr++;
            }
            data = *addr;
#if 1
            obd_conf[i  ] = (data>> 0)&0xFF;
#else
            obd_conf[i  ] = (data>>24)&0xFF;
#endif
            i += 1;
            addr +=4;
        }
    }
}

} // end of namespace aif
