#include <aif/bodyPoseEstimation/personDetect/yolov3_yc_v1/Yolov3YCV1Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <cstdio>
#include <boost/algorithm/string/predicate.hpp>

namespace aif {

constexpr int Yolov3YCV1Detector::tcnt_init[];

Yolov3YCV1Detector::Yolov3YCV1Detector(const std::string &modelPath)
    : PersonDetector(modelPath)
    , m_obd_result{0,}
    , m_bodyResult{0,}
    , m_faceResult{0,}
    , m_ImgResizingScale(1.f)
    , m_leftBorder(0)
    , m_topBorder(0)
    , m_IsBodyDetect(true)
    , m_frameId(0)
{
}

Yolov3YCV1Detector::~Yolov3YCV1Detector()
{
}

std::shared_ptr<DetectorParam> Yolov3YCV1Detector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<Yolov3YCV1Param>();
    return param;
}

void Yolov3YCV1Detector::setModelInfo(TfLiteTensor *inputTensor)
{
    if (inputTensor == nullptr) {
        Loge("inputTensor ptr is null");
        return;
    }
    t_aif_modelinfo inputInfo;
    inputInfo.inputSize = inputTensor->dims->size;    //inputSize
    inputInfo.batchSize = 1;                          // batchSize
    inputInfo.height    = inputTensor->dims->data[0]; // height
    inputInfo.width     = inputTensor->dims->data[1]; // width
    inputInfo.channels  = inputTensor->dims->data[2]; // channels

    m_modelInputInfo.emplace_back(inputInfo);

    m_modelInfo = m_modelInputInfo[m_modelInputInfo.size()-1]; // print last
    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);
}

t_aif_status Yolov3YCV1Detector::fillInputTensor(const cv::Mat &img) /* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv image!!");
        }

        if (m_interpreter == nullptr) {
            throw std::runtime_error("yolov4.tflite interpreter not initialized!!");
        }

        cv::Mat img_resized;
        if (isRoiValid(img.cols, img.rows)) {
            cv::Mat roi_img = img(cv::Rect(mOrigImgRoiX, mOrigImgRoiY, mOrigImgRoiWidth, mOrigImgRoiHeight));
            getPaddedImage(roi_img, img_resized);
        } else {
            getPaddedImage(img, img_resized);
        }

        TRACE("resized size: ", img_resized.size());
        if (img_resized.rows != m_modelInputInfo[0].height || img_resized.cols != m_modelInputInfo[0].width) {
            throw std::runtime_error("image resize failed!!");
        }

        cv::Mat ycrcb;
        cv::cvtColor(img_resized, ycrcb, cv::COLOR_RGB2YCrCb);
        std::vector<cv::Mat> ycrcb_planes;
        cv::split(ycrcb, ycrcb_planes);
        cv::Mat input_y = ycrcb_planes[0]; // final input for model
        cv::Mat unmerged_crcb[2] = {ycrcb_planes[1], ycrcb_planes[1]};
        cv::Mat merged_crcb;
        cv::merge(unmerged_crcb, 2, merged_crcb);
        cv::Mat input_uv; // final input for model
        cv::resize(merged_crcb, input_uv, cv::Size(m_modelInputInfo[1].width, m_modelInputInfo[1].height), 0, 0, cv::INTER_LINEAR); // AREA
        // memoryDump(input_y.data, "./y_input.bin", m_modelInputInfo[0].width * m_modelInputInfo[0].height * m_modelInputInfo[0].channels * sizeof(uint8_t));
        // memoryDump(input_uv.data, "./uv_input.bin", m_modelInputInfo[1].width * m_modelInputInfo[1].height * m_modelInputInfo[1].channels * sizeof(uint8_t));

        uint8_t *inputTensor_f = m_interpreter->typed_input_tensor<uint8_t>(0);
        uint8_t *inputTensor_s = m_interpreter->typed_input_tensor<uint8_t>(1);
        // memoryRestore(inputTensor_f, "./image_y.bin");
        // memoryRestore(inputTensor_s, "./image_uv.bin");
        std::memcpy(inputTensor_f, input_y.ptr<uint8_t>(), m_modelInputInfo[0].width * m_modelInputInfo[0].height * m_modelInputInfo[0].channels * sizeof(uint8_t));
        std::memcpy(inputTensor_s, input_uv.ptr<uint8_t>(), m_modelInputInfo[1].width * m_modelInputInfo[1].height * m_modelInputInfo[1].channels * sizeof(uint8_t));

        return kAifOk;
    } catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }

    return kAifOk;
}

t_aif_status Yolov3YCV1Detector::preProcessing()
{
    try
    {
        std::shared_ptr<Yolov3YCV1Param> param = std::dynamic_pointer_cast<Yolov3YCV1Param>(m_param);
        if (param == nullptr) {
            throw std::runtime_error("failed to convert DetectorParam to Yolov3YCV1Param");
        }

        if (boost::iequals(param->detectObject, "body")) {
            m_IsBodyDetect = true;
        } else if (boost::iequals(param->detectObject, "face")) {
            m_IsBodyDetect = false;
        } else {
            throw std::runtime_error("invalid param, not body nor face");
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
    }
    catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }
    return kAifOk;
}

t_aif_status Yolov3YCV1Detector::postProcessing(const cv::Mat &img, std::shared_ptr<Descriptor> &descriptor)
{
    try {
        Stopwatch sw;
        sw.start();
        const std::vector<int> &outputs = m_interpreter->outputs();
        TfLiteTensor *output_0 = m_interpreter->tensor(outputs[0]); // obd_mb [30,18,48]
        TfLiteTensor *output_1 = m_interpreter->tensor(outputs[1]); // obd_lb [15,9,48]
        t_pqe_obd_result *result = nullptr;

        auto lb_size = int(output_1->dims->data[0]) * int(output_1->dims->data[1]) * int(output_1->dims->data[2]);
        auto mb_size = int(output_0->dims->data[0]) * int(output_0->dims->data[1]) * int(output_0->dims->data[2]);

        char* data_0= reinterpret_cast<char*>(output_1->data.data);
        char* data_1= reinterpret_cast<char*>(output_0->data.data);

        char* lb_data = new char[lb_size];
        char* mb_data = new char[mb_size];

        std::memcpy(lb_data, data_0, lb_size * sizeof(char));
        std::memcpy(mb_data, data_1, mb_size* sizeof(char));

        for (int idx = 0; idx < lb_size; idx ++){
            lb_data[idx] +=128;
        }

        for (int idx = 0; idx < mb_size; idx ++){
            mb_data[idx] +=128;
        }

        if (m_IsBodyDetect) {
            result = &m_bodyResult;
        } else {
            result = &m_faceResult;
        }

        unsigned short obd_lb[SZ_CONV_LBBOX]; // lb_data
	    unsigned short obd_mb[SZ_CONV_MBBOX]; // mb_data

        RD_Result(obd_lb, reinterpret_cast<unsigned int*>(lb_data), SZ_LBBOX_W, SZ_LBBOX_H);
	    RD_Result(obd_mb, reinterpret_cast<unsigned int*>(mb_data), SZ_MBBOX_W, SZ_MBBOX_H);

        delete[] lb_data;
        delete[] mb_data;

        OBD_ComputeResult(obd_lb, obd_mb);
        transform2OriginCoord(img, *result);

        std::shared_ptr<Yolov3YCV1Param> param = std::dynamic_pointer_cast<Yolov3YCV1Param>(m_param);
        if (param == nullptr) {
            throw std::runtime_error("failed to convert DetectorParam to Yolov3YCV1Param");
        }

        std::shared_ptr<Yolov3YCV1Descriptor> yolov3Descriptor = std::dynamic_pointer_cast<Yolov3YCV1Descriptor>(descriptor);
        if (yolov3Descriptor == nullptr) {
            throw std::runtime_error("failed to convert Descriptor to Yolov3YCV1Descriptor");
        }

        std::vector<std::pair<BBox, float>> finalBboxList;
        for (int i = 0; i < result->res_cnt; i++) {
            BBox finalBbox(img.cols, img.rows);
            finalBbox.addXyxy(result->pos_x0[i], result->pos_y0[i],
                                result->pos_x1[i], result->pos_y1[i], false);
            finalBboxList.emplace_back(finalBbox, result->res_val[i]);
        }

        // sort boxes with area descending order
        std::sort(finalBboxList.begin(), finalBboxList.end(),
                    [](const std::pair<BBox, float> &a, const std::pair<BBox, float> &b)
                    {
                        return (a.first.width * a.first.height) > (b.first.width * b.first.height);
                    });

        for (int i = 0; (i < finalBboxList.size()) && (i < param->numMaxPerson); i++) {
            if (!checkUpdate(i, finalBboxList[i].first)) {
                finalBboxList[i].second = m_prevBboxList[i].second; // score
                finalBboxList[i].first = m_prevBboxList[i].first;   // bbox
            }

            float score = (finalBboxList[i].second / OBD_SCORE_MAX);
            const BBox &finalBbox = finalBboxList[i].first;

            if (m_IsBodyDetect) {
                if (m_GTBBoxes.size() > m_frameId) {
                    auto gtBBox = m_GTBBoxes[m_frameId]; // pair
                    if (gtBBox.second.width > 0 && gtBBox.second.height > 0) {
                        yolov3Descriptor->addPerson(score, gtBBox.second);
                        Logi(__func__, " use GT Box !! ", gtBBox.first, ", ", gtBBox.second);
                    }
                } else {
                    yolov3Descriptor->addPerson(score, finalBbox);
                }
            } else {
                /* normalize 0~1 */
                yolov3Descriptor->addFace(score,
                                        finalBbox.xmin / img.cols, finalBbox.ymin / img.rows,
                                        finalBbox.width / img.cols, finalBbox.height / img.rows,
                                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                // cv::Mat image = cv::imread("./saved_image.png", cv::IMREAD_COLOR);
                // cv::Point lb(finalBbox.xmin, finalBbox.ymin);
                // cv::Point tr(finalBbox.width+finalBbox.xmin, finalBbox.height+finalBbox.ymin);
                // cv::rectangle(image, lb, tr, cv::Scalar(0, 255, 0), 2);
                // cv::imwrite("./saved_image_out.png", image);
            }
        }
        m_frameId++;
        m_prevBboxList = finalBboxList;
        TRACE("postProcessing(): ", sw.getMs(), "ms");
        sw.stop();
    } catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }

    return kAifOk;
}

void Yolov3YCV1Detector::getPaddedImage(const cv::Mat &src, cv::Mat &dst)
{
    float srcW = src.size().width;
    float srcH = src.size().height;
    int dstW = 0;
    int dstH = 0;

    // r : 0.5625 ( 270/480 < 480/640)
    // r : 0.28125 ( 270/960 vs. 480/1280)
    m_ImgResizingScale = std::min(m_modelInputInfo[0].width / (srcW * 1.0), m_modelInputInfo[0].height / (srcH * 1.0));
    TRACE(__func__, " m_ImgResizingScale = ", m_ImgResizingScale);

    int width = srcW * m_ImgResizingScale;
    int height = srcH * m_ImgResizingScale;
    // 360 x 270
    // 360 x 270

    cv::Mat inputImg;
    cv::resize(src, inputImg, cv::Size(width, height), 0, 0, cv::INTER_LINEAR); // AREA

    m_leftBorder = 0;
    m_topBorder = 0;
    if (m_modelInputInfo[0].width != width)
        m_leftBorder = (m_modelInputInfo[0].width - width) / 2;
    if (m_modelInputInfo[0].height != height)
        m_topBorder = (m_modelInputInfo[0].height - height) / 2;

    int rightBorder = m_modelInputInfo[0].width - width - m_leftBorder;
    int bottomBorder = m_modelInputInfo[0].height - height - m_topBorder;

    TRACE(__func__, "lb, tb, rb, bb: ", m_leftBorder, ", ", m_topBorder, ", ", rightBorder, " , ", bottomBorder);

    cv::copyMakeBorder(inputImg, dst, m_topBorder, bottomBorder, m_leftBorder, rightBorder, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    m_paddedSize = cv::Size(
        srcW * (static_cast<float>(m_modelInputInfo[0].width) / width),
        srcH * (static_cast<float>(m_modelInputInfo[0].height) / height));
}

void Yolov3YCV1Detector::transform2OriginCoord(const cv::Mat &img, t_pqe_obd_result &result)
{
    int img_width = img.cols;
    int img_height = img.rows;
    auto off_x = 0.0f;
    auto off_y = 0.0f;
    auto roi_img_width = img_width;
    auto roi_img_height = img_height;
    if (isRoiValid(img_width, img_height)) {
        off_x = static_cast<float>(mOrigImgRoiX);
        off_y = static_cast<float>(mOrigImgRoiY);
        roi_img_width = static_cast<float>(mOrigImgRoiWidth);
        roi_img_height = static_cast<float>(mOrigImgRoiHeight);
    }

    if (std::abs(m_ImgResizingScale - 1.f) < aif::EPSILON) {
        m_ImgResizingScale = std::min(m_modelInputInfo[0].width / (roi_img_width * 1.0), m_modelInputInfo[0].height / (roi_img_height * 1.0));
    }

    TRACE(__func__, "m_ImgResizingScale = ", m_ImgResizingScale);
    // scale : 0.5625 ( 270/480 < 480/640)

    TRACE("---------------BEFORE------------");
    if (m_IsBodyDetect) {
        TRACE("[BODY_Result] res_cnt = ", m_bodyResult.res_cnt);
    } else {
        TRACE("[FACE_Result] res_cnt = ", m_faceResult.res_cnt);
    }
    TRACE("---------------------------");

    for (int idx = 0; idx < result.res_cnt; idx++) {
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
        int x0 = ((result.pos_x0[i] - m_leftBorder) / m_ImgResizingScale) + off_x;
        int y0 = ((result.pos_y0[i] - m_topBorder) / m_ImgResizingScale) + off_y;
        int x1 = ((result.pos_x1[i] - m_leftBorder) / m_ImgResizingScale) + off_x;
        int y1 = ((result.pos_y1[i] - m_topBorder) / m_ImgResizingScale) + off_y;

        result.pos_x0[i] = std::max(std::min(x0, img_width - 1), 0);
        result.pos_y0[i] = std::max(std::min(y0, img_height - 1), 0);
        result.pos_x1[i] = std::max(std::min(x1, img_width - 1), 0);
        result.pos_y1[i] = std::max(std::min(y1, img_height - 1), 0);
    }

    TRACE("-------------AFTER--------------");
    if (m_IsBodyDetect) {
        TRACE("[BODY_Result] res_cnt = ", m_bodyResult.res_cnt);
    } else {
        TRACE("[FACE_Result] res_cnt = ", m_faceResult.res_cnt);
    }
    TRACE("---------------------------");

    for (int idx = 0; idx < result.res_cnt; idx++) {
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

void Yolov3YCV1Detector::OBD_ComputeResult(unsigned short* obd_lb_addr, unsigned short* obd_mb_addr)
{
    std::vector<BBox> bbox;
    CV_BOX(BoxType::LB_BOX, obd_lb_addr, bbox);
    CV_BOX(BoxType::MB_BOX, obd_mb_addr, bbox);

    TRACE(" num_bbox [", bbox.size(), "] : MAX_BOX[", MAX_BOX, "]");

    // NMS
    int num_nms = 0;
    std::vector<int> res_nms;
    if (bbox.size() <= MAX_BOX) {
        res_nms = fw_nms(bbox);
        num_nms = GET_CLIP(res_nms.size(), MAX_BOX);
    }

    Classify_OBD_Result(bbox, num_nms, res_nms);

    if (m_IsBodyDetect == false) {
        FaceMatching();
    }
}

void Yolov3YCV1Detector::CV_BOX(BoxType boxType, unsigned short* obd_addr, std::vector<BBox> &bbox)
{
    std::shared_ptr<Yolov3YCV1Param> param = std::dynamic_pointer_cast<Yolov3YCV1Param>(m_param);
    if (param == nullptr) {
        Loge(__func__, "failed to convert DetectorParam to Yolov3YCV1Param");
        return;
    }

    int j, i, c, cnt;
    int BX0, BY0, BX1, BY1, CO;
    int BX_partial, BY_partial;
    int box_h, box_w, partial_shift, boxIdx;
    int numBox_before = bbox.size();

    if (boxType == BoxType::LB_BOX) {
        box_h = param->lbbox_h;
        box_w = param->lbbox_w;
        partial_shift = 5;
        boxIdx = 0;
    }
    else if (boxType == BoxType::MB_BOX) {
        box_h = param->mbbox_h;
        box_w = param->mbbox_w;
        partial_shift = 4;
        boxIdx = 1;
    }
    else {
        throw std::runtime_error("invalid boxType ");
    }

    cnt = 0;
    for (j = 0; j < box_h; j++) {
        BY_partial = j << partial_shift;
        for (i = 0; i < box_w; i++) {
            BX_partial = i << partial_shift;
            for (c = 0; c < param->box_size; c++) {
                BX0 = cal_pos(BX_partial, LUT_EXP[boxIdx], obd_addr[cnt],     0, param->stride[boxIdx]);
                BY0 = cal_pos(BY_partial, LUT_EXP[boxIdx], obd_addr[cnt + 1], 0, param->stride[boxIdx]);
                BX1 = cal_pos(BX_partial, LUT_EXP[boxIdx], obd_addr[cnt + 2], 1, param->stride[boxIdx]);
                BY1 = cal_pos(BY_partial, LUT_EXP[boxIdx], obd_addr[cnt + 3], 1, param->stride[boxIdx]);

                BX0 = (BX0 > m_modelInputInfo[0].width)  ? m_modelInputInfo[0].width  : BX0;
                BY0 = (BY0 > m_modelInputInfo[0].height) ? m_modelInputInfo[0].height : BY0;
                BX1 = (BX1 > m_modelInputInfo[0].width)  ? m_modelInputInfo[0].width  : BX1;
                BY1 = (BY1 > m_modelInputInfo[0].height) ? m_modelInputInfo[0].height : BY1;
                // std::cout << "checking : " << BX0<<", "<<BY0<<","<<BX1<<","<<BY1 <<","<< m_modelInputInfo[0].height <<","<<param->stride[boxIdx]<<obd_addr[cnt]<<","<<LUT_EXP[boxIdx][254]<< std::endl;

                unsigned short max = 0;
                int max_id = -1;
                for (int n = 0; n < param->class_size; n++) {
                    if (obd_addr[cnt + 5 + n] > max) {
                        max = obd_addr[cnt + 5 + n];
                        max_id = n;
                    }
                }

                CO = cal_confidence(LUT_CONF[boxIdx], obd_addr[cnt + 4], max);
                // std::cout << "checking co: " << CO << std::endl;
                if (CO > param->thresh_score[1]) /* low threshold */ {
                    bbox.emplace_back(BX0, BY0, BX1, BY1, CO, max_id);
                    // Logi(__func__, " bbox: ", BX0, " " ,BY0," ", BX1," ", BY1, " ",CO, " ", max_id);
                    // printf("%3d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n", boxIdx, BX0, BY0, BX1, BY1, max_id, CO);
                }
                cnt += 15;
            }
        }
    }
    TRACE(__FUNCTION__, ": ", bbox.size() - numBox_before);
}

std::vector<int> Yolov3YCV1Detector::fw_nms(std::vector<BBox> &bbox)
{
    std::vector<int> res_nms;
    std::shared_ptr<Yolov3YCV1Param> param = std::dynamic_pointer_cast<Yolov3YCV1Param>(m_param);
    if (param == nullptr) {
        Loge(__func__, "failed to convert DetectorParam to Yolov3YCV1Param");
        return res_nms;
    }

    // sort
    int num_sort = 0;
    int res_sort[MAX_BOX];

    for (int idx = 0; idx < bbox.size(); idx++) {
        int c_idx = idx;
        int c_score = bbox[c_idx].c0;

        for (int idx_sort = 0; idx_sort < num_sort; idx_sort++) {
            int t_idx = res_sort[idx_sort];
            int t_score = bbox[t_idx].c0;

            if (c_score > t_score) { // swap
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
        int c_idx       = res_sort[idx_sort];
        long int c_area = res_area[c_idx];
        int c_score     = bbox[c_idx].c0;
        int c_class     = bbox[c_idx].c1;
        int th_score_avg = (c_score * 4) >> 3;
        int c_keep = keep_flag[c_idx];
        if (c_keep == 0) continue;
        long int cnt_sim = 0;
        long int sum_x0  = 0;
        long int sum_y0  = 0;
        long int sum_x1  = 0;
        long int sum_y1  = 0;

        for (int idx_nms = idx_sort + 1; idx_nms < num_sort; idx_nms++) {
            int t_idx       = res_sort[idx_nms];
            long int t_area = res_area[t_idx];
            int t_score     = bbox[t_idx].c0;
            int t_class     = bbox[t_idx].c1;
            int t_keep      = keep_flag[t_idx];
            if (t_keep == 0) continue;

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

                long int th_area_avg = (u_area * param->thresh_iou_sc_avg) >> 8;
                if ((i_area > th_area_avg) && (t_score > th_score_avg)) {
                    cnt_sim += 1;
                    sum_x0 += bbox[t_idx].xmin - bbox[c_idx].xmin;
                    sum_y0 += bbox[t_idx].ymin - bbox[c_idx].ymin;
                    sum_x1 += bbox[t_idx].xmax - bbox[c_idx].xmax;
                    sum_y1 += bbox[t_idx].ymax - bbox[c_idx].ymax;
                }
            }
        }

        if (cnt_sim > 0) {
            bbox[c_idx].xmin += sum_x0 / cnt_sim;
            bbox[c_idx].ymin += sum_y0 / cnt_sim;
            bbox[c_idx].xmax += sum_x1 / cnt_sim;
            bbox[c_idx].ymax += sum_y1 / cnt_sim;
        }

        if (c_keep) {
            res_nms.push_back(c_idx);
        }
    }

    TRACE(__FUNCTION__, ": ", res_nms.size());
    return res_nms;
}

void Yolov3YCV1Detector::Classify_OBD_Result(const std::vector<BBox> &bbox, int num_nms, std::vector<int> &res_nms)
{
    m_obd_result = {0,};
    m_bodyResult = {0,};
    m_faceResult = {0,};

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

        selState = ((_class == eOBD_PERSON) || (_class == eOBD_FACE)) ? 0 : 1; // objects execept for person, face

        if (selState != 0 && m_obd_result.res_cnt < OBD_RESULT_NUM) {
            /* other objects except for person , face */
            m_obd_result.res_cls[m_obd_result.res_cnt] = _class;
            m_obd_result.pos_x0[m_obd_result.res_cnt]  = _x0;
            m_obd_result.pos_x1[m_obd_result.res_cnt]  = _x1;
            m_obd_result.pos_y0[m_obd_result.res_cnt]  = _y0;
            m_obd_result.pos_y1[m_obd_result.res_cnt]  = _y1;

            if (_x0 != _x1 && _y0 != _y1) {
                m_obd_result.res_val[m_obd_result.res_cnt] = _score;
                m_obd_result.tpr_cnt[m_obd_result.res_cnt] = tcnt_init[_class];
                m_obd_result.res_cnt++;
            }

            // printf("%u:%2d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n", m_obd_result.res_cnt, idx, _x0, _x1, _y0, _y1, _class, _score);
            if (m_obd_result.res_cnt >= OBD_RESULT_NUM) break;
        }
        //			else //only for person & face
        {
            if (_class == eOBD_PERSON && m_bodyResult.res_cnt < OBD_RESULT_NUM) {
                m_bodyResult.res_cls[m_bodyResult.res_cnt] = _class;
                m_bodyResult.pos_x0[m_bodyResult.res_cnt]  = _x0;
                m_bodyResult.pos_x1[m_bodyResult.res_cnt]  = _x1;
                m_bodyResult.pos_y0[m_bodyResult.res_cnt]  = _y0;
                m_bodyResult.pos_y1[m_bodyResult.res_cnt]  = _y1;

                if (_x0 != _x1 && _y0 != _y1) {
                    m_bodyResult.res_val[m_bodyResult.res_cnt] = _score;
                    m_bodyResult.tpr_cnt[m_bodyResult.res_cnt] = tcnt_init[_class];
                    m_bodyResult.res_cnt++;
                }
                // printf("person%u:%2d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n",m_bodyResult.res_cnt, idx, _x0, _x1, _y0, _y1, _class, _score);
            } else if (_class == eOBD_FACE && m_faceResult.res_cnt < OBD_RESULT_NUM) {
                m_faceResult.res_cls[m_faceResult.res_cnt] = _class;
                m_faceResult.pos_x0[m_faceResult.res_cnt]  = _x0;
                m_faceResult.pos_x1[m_faceResult.res_cnt]  = _x1;
                m_faceResult.pos_y0[m_faceResult.res_cnt]  = _y0;
                m_faceResult.pos_y1[m_faceResult.res_cnt]  = _y1;

                if (_x0 != _x1 && _y0 != _y1) {
                    m_faceResult.res_val[m_faceResult.res_cnt] = _score;
                    m_faceResult.tpr_cnt[m_faceResult.res_cnt] = tcnt_init[_class];

                    // printf("face%u:%2d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n", m_faceResult.res_cnt, idx, _x0, _x1, _y0, _y1, _class, _score);
                    m_faceResult.res_cnt++;
                }
            }

            if (m_bodyResult.res_cnt >= OBD_RESULT_NUM && m_faceResult.res_cnt >= OBD_RESULT_NUM) break;
        }
    }

    if (m_obd_result.res_cnt < 0 || m_obd_result.res_cnt > OBD_RESULT_NUM) m_obd_result.res_cnt = 0;
    if (m_bodyResult.res_cnt < 0 || m_bodyResult.res_cnt > OBD_RESULT_NUM) m_bodyResult.res_cnt = 0;
    if (m_faceResult.res_cnt < 0 || m_faceResult.res_cnt > OBD_RESULT_NUM) m_faceResult.res_cnt = 0;
}

void Yolov3YCV1Detector::FaceMatching()
{
    std::shared_ptr<Yolov3YCV1Param> param = std::dynamic_pointer_cast<Yolov3YCV1Param>(m_param);
    if (param == nullptr) {
        Loge(__func__, "failed to convert DetectorParam to Yolov3YCV1Param");
        return;
    }

    // face-body matching
    int idx, fid, bid, oid, max_id;
    int i_x0, i_y0, i_x1, i_y1;
    int i_dist, max_dist, i_conf, max_conf;
    long int f_area, i_area, max_area, th_area;
    int th_score;

    t_pqe_obd_result faceInput;
    faceInput.res_cnt = GET_CLIP((m_faceResult).res_cnt, OBD_RESULT_NUM);
    (m_faceResult).res_cnt = 0;
    for (idx = 0; idx < faceInput.res_cnt; idx++) {
        faceInput.res_val[idx] = (m_faceResult).res_val[idx];
        faceInput.res_cls[idx] = (m_faceResult).res_cls[idx];
        faceInput.tpr_cnt[idx] = (m_faceResult).tpr_cnt[idx];
        faceInput.pos_x0[idx] = (m_faceResult).pos_x0[idx];
        faceInput.pos_x1[idx] = (m_faceResult).pos_x1[idx];
        faceInput.pos_y0[idx] = (m_faceResult).pos_y0[idx];
        faceInput.pos_y1[idx] = (m_faceResult).pos_y1[idx];

        (m_faceResult).res_val[idx] = 0;
        (m_faceResult).res_cls[idx] = 0;
        (m_faceResult).tpr_cnt[idx] = 0;
        (m_faceResult).pos_x0[idx] = 0;
        (m_faceResult).pos_x1[idx] = 0;
        (m_faceResult).pos_y0[idx] = 0;
        (m_faceResult).pos_y1[idx] = 0;
    }

    for (fid = 0; fid < faceInput.res_cnt; fid++) {
        f_area   = std::max(faceInput.pos_x1[fid] - faceInput.pos_x0[fid], 0) * std::max(faceInput.pos_y1[fid] - faceInput.pos_y0[fid], 0);
        th_area  = (f_area * 9) >> 4;
        max_area = 0;
        max_dist = 0;
        max_conf = 0;
        max_id   = -1;

        for (bid = 0; bid < (m_bodyResult).res_cnt; bid++) {
            i_x0 = std::max(faceInput.pos_x0[fid], (m_bodyResult).pos_x0[bid]);
            i_y0 = std::max(faceInput.pos_y0[fid], (m_bodyResult).pos_y0[bid]);
            i_x1 = std::min(faceInput.pos_x1[fid], (m_bodyResult).pos_x1[bid]);
            i_y1 = std::min(faceInput.pos_y1[fid], (m_bodyResult).pos_y1[bid]);
            i_area = std::max(i_x1 - i_x0, 0) * std::max(i_y1 - i_y0, 0);
            i_dist = abs(faceInput.pos_x0[fid] + faceInput.pos_x1[fid] - (m_bodyResult).pos_x0[bid] - (m_bodyResult).pos_x1[bid]);
            i_conf = (m_bodyResult).res_val[bid];

            if (i_area > max_area) {
                max_area = i_area;
                max_dist = i_dist;
                max_conf = i_conf;
                max_id = bid;
            } else if ((max_id != -1) && (i_area == max_area) && (i_dist < max_dist)) {
                // select near x
                max_area = i_area;
                max_dist = i_dist;
                max_conf = i_conf;
                max_id = bid;
            }
        }

        if ((max_id != -1) && (max_area > th_area) && (max_conf > param->thresh_score[0])) {
            if (((m_faceResult).res_cnt >= 0) && ((m_faceResult).res_cnt < OBD_RESULT_NUM)) {
                (m_faceResult).res_val[(m_faceResult).res_cnt] = faceInput.res_val[fid];
                (m_faceResult).res_cls[(m_faceResult).res_cnt] = faceInput.res_cls[fid];
                (m_faceResult).tpr_cnt[(m_faceResult).res_cnt] = faceInput.tpr_cnt[fid];
                (m_faceResult).pos_x0[(m_faceResult).res_cnt] = faceInput.pos_x0[fid];
                (m_faceResult).pos_x1[(m_faceResult).res_cnt] = faceInput.pos_x1[fid];
                (m_faceResult).pos_y0[(m_faceResult).res_cnt] = faceInput.pos_y0[fid];
                (m_faceResult).pos_y1[(m_faceResult).res_cnt] = faceInput.pos_y1[fid];

                (m_faceResult).res_cnt = std::min(std::max((m_faceResult).res_cnt + 1, 0), OBD_RESULT_NUM);
            }
        }
        else {
            max_id = -1;
            // No person-face matching
            for (oid = 0; oid < m_obd_result.res_cnt; oid++) {
                if ((m_obd_result.res_cls[oid] == eOBD_CAT) || (m_obd_result.res_cls[oid] == eOBD_DOG)) {
                    i_x0 = std::max(faceInput.pos_x0[fid], (m_obd_result).pos_x0[oid]);
                    i_y0 = std::max(faceInput.pos_y0[fid], (m_obd_result).pos_y0[oid]);
                    i_x1 = std::min(faceInput.pos_x1[fid], (m_obd_result).pos_x1[oid]);
                    i_y1 = std::min(faceInput.pos_y1[fid], (m_obd_result).pos_y1[oid]);
                    i_area = std::max(i_x1 - i_x0, 0) * std::max(i_y1 - i_y0, 0);
                    i_dist = std::abs(faceInput.pos_x0[fid] + faceInput.pos_x1[fid] - (m_obd_result).pos_x0[oid] - (m_obd_result).pos_x1[oid]);
                    i_conf = (m_obd_result).res_val[bid];

                    if (i_area > max_area) {
                        max_area = i_area;
                        max_dist = i_dist;
                        max_conf = i_conf;
                        max_id = oid;
                        // printf("max id: %d, area: %ld (%ld) %s\n", max_id, max_area, th_area, (max_area > th_area ? "" : "X"));
                    } else if ((max_id != -1) && (i_area == max_area) && (i_dist < max_dist)) {
                        // select near x
                        max_area = i_area;
                        max_dist = i_dist;
                        max_conf = i_conf;
                        max_id = oid;
                        // printf("max id: %d, area: %ld (%ld) %s\n", max_id, max_area, th_area, (max_area > th_area ? "" : "X"));
                    }
                }
            }

            th_score = ((max_id != -1) && (max_area > th_area) && (max_conf > param->thresh_score[0])) ? param->thresh_score[3] : 40000; // TH_SCORE_M;

            if (faceInput.res_val[fid] > th_score) {
                if (((m_faceResult).res_cnt >= 0) && ((m_faceResult).res_cnt < OBD_RESULT_NUM)) {
                    (m_faceResult).res_val[(m_faceResult).res_cnt] = faceInput.res_val[fid];
                    (m_faceResult).res_cls[(m_faceResult).res_cnt] = faceInput.res_cls[fid];
                    (m_faceResult).tpr_cnt[(m_faceResult).res_cnt] = faceInput.tpr_cnt[fid];
                    (m_faceResult).pos_x0[(m_faceResult).res_cnt] = faceInput.pos_x0[fid];
                    (m_faceResult).pos_x1[(m_faceResult).res_cnt] = faceInput.pos_x1[fid];
                    (m_faceResult).pos_y0[(m_faceResult).res_cnt] = faceInput.pos_y0[fid];
                    (m_faceResult).pos_y1[(m_faceResult).res_cnt] = faceInput.pos_y1[fid];

                    (m_faceResult).res_cnt = std::min(std::max((m_faceResult).res_cnt + 1, 0), OBD_RESULT_NUM);
                }
            }
        }
    }
}

bool Yolov3YCV1Detector::checkUpdate(int index, const BBox &currBbox)
{
    std::shared_ptr<Yolov3YCV1Param> param = std::dynamic_pointer_cast<Yolov3YCV1Param>(m_param);
    if (param == nullptr) {
        Loge(__func__, "failed to convert DetectorParam to Yolov3YCV1Param");
        return false;
    }

    if (m_prevBboxList.size() == 0) {
        Logd("UPDATE!!!!!!!!!!!!!!!!!");
        return true;
    }
    Logd("m_prevBboxList.size() ", m_prevBboxList.size());

    const BBox &prevBbox = m_prevBboxList[index].first;
    cv::Rect2f prevRect(prevBbox.xmin, prevBbox.ymin, prevBbox.width, prevBbox.height);
    cv::Rect2f curRect(currBbox.xmin, currBbox.ymin, currBbox.width, currBbox.height);

    if (index < m_prevBboxList.size() && isIOUOver(curRect, prevRect, param->thresh_iou_update)) {
        return false;
    }
    Logd("UPDATE!!!!!!!!!!!!!!!!!");
    return true;
}

void Yolov3YCV1Detector::RD_Result(unsigned short* obd_lb, unsigned int* addr,unsigned int size_x, unsigned int size_y)
{
    int ix,iy,ic,i;
    unsigned int data;

    i = 0;
    for(iy = 0; iy < size_y; iy++) {
        for(ix = 0; ix < size_x; ix++) {
            for(ic = 0 ; ic < 44; ic += 4) {
                data = *addr;
#if 1
                obd_lb[i  ] = (data>> 0)&0xFF;
                obd_lb[i+1] = (data>> 8)&0xFF;
                obd_lb[i+2] = (data>>16)&0xFF;
                obd_lb[i+3] = (data>>24)&0xFF;
#else
                obd_lb[i  ] = (data>>24)&0xFF;
                obd_lb[i+1] = (data>>16)&0xFF;
                obd_lb[i+2] = (data>> 8)&0xFF;
                obd_lb[i+3] = (data    )&0xFF;
#endif
                i += 4;
               addr++;
            }
            data = *addr;
#if 1
            obd_lb[i  ] = (data>> 0)&0xFF;
#else
            obd_lb[i  ] = (data>>24)&0xFF;
#endif
            i += 1;
            addr ++;
        }
    }
}

} // end of namespace aif
