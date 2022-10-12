#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <cstdio>

namespace aif {

constexpr int Yolov3Detector::tcnt_init[];


Yolov3Detector::Yolov3Detector(const std::string& modelPath)
    : Detector(modelPath)
    , m_obd_result{0,}
    , m_bodyResult{0,}
    , m_faceResult{0,}
    , m_ImgResizingScale(1.f)
    , m_leftBorder(0)
    , m_topBorder(0)
{
}

Yolov3Detector::~Yolov3Detector()
{
}

std::shared_ptr<DetectorParam> Yolov3Detector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<Yolov3Param>();
    return param;
}

void Yolov3Detector::setModelInfo(TfLiteTensor* inputTensor)
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

t_aif_status Yolov3Detector::fillInputTensor(const cv::Mat& img)/* override*/
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

        cv::Mat img_resized;// = staticResize(img, width, height);
        getPaddedImage(img, img_resized);

        TRACE("resized size: ", img_resized.size());
        if (img_resized.rows != height || img_resized.cols != width) {
            throw std::runtime_error("image resize failed!!");
        }

        img_resized.convertTo(img_resized, CV_8U);

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                const auto &rgb = img_resized.at<cv::Vec3b>(i, j);
                m_interpreter->typed_input_tensor<uint8_t>(0)[i * width * channels + j * channels + 0] = rgb[2];
                m_interpreter->typed_input_tensor<uint8_t>(0)[i * width * channels + j * channels + 1] = rgb[1];
                m_interpreter->typed_input_tensor<uint8_t>(0)[i * width * channels + j * channels + 2] = rgb[0];
            }
        }
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

t_aif_status Yolov3Detector::preProcessing()
{
    return kAifOk;
}

t_aif_status Yolov3Detector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    try {
        /* TODO: dequantization and output numbering */

        Stopwatch sw;
        sw.start();
        const std::vector<int> &outputs = m_interpreter->outputs();
        TfLiteTensor *output_1 = m_interpreter->tensor(outputs[1]);
        TfLiteTensor *output_2 = m_interpreter->tensor(outputs[2]);

        OBD_ComputeResult(output_1->data.uint8, output_2->data.uint8);

        transform2OriginCoord(img, m_bodyResult);

        std::shared_ptr<Yolov3Descriptor> yolov3Descriptor = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);
        for (int i=0; i< m_bodyResult.res_cnt; i++) {
            BBox finalBbox( img.cols , img.rows );
            finalBbox.addXyxy(m_bodyResult.pos_x0[i], m_bodyResult.pos_y0[i],
                         m_bodyResult.pos_x1[i], m_bodyResult.pos_y1[i], false);

            yolov3Descriptor->addPerson(m_bodyResult.res_val[i], finalBbox);
        }
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

void
Yolov3Detector::getPaddedImage(const cv::Mat& src, cv::Mat& dst)
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
    cv::resize(src, inputImg, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);

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


void
Yolov3Detector::transform2OriginCoord(const cv::Mat& img, t_pqe_obd_result &result)
{
    int img_width = img.cols;
    int img_height = img.rows;
    auto off_x = 0.0f;
    auto off_y = 0.0f;

    if (m_ImgResizingScale == 1.f) {
        m_ImgResizingScale = std::min( m_modelInfo.width / (img_width * 1.0), m_modelInfo.height / (img_height * 1.0) );
    }

    TRACE(__func__, "m_ImgResizingScale = " , m_ImgResizingScale);
    // scale : 0.5625 ( 270/480 < 480/640)

    TRACE("---------------BEFORE------------");
    TRACE("[BODY_Result] res_cnt = ", m_bodyResult.res_cnt);
    TRACE("---------------------------");

    for(int idx = 0; idx < m_bodyResult.res_cnt; idx++)
    {
        TRACE(" res_val[", idx, "] = ", m_bodyResult.res_val[idx]);
        TRACE(" res_cls[", idx, "] = ", m_bodyResult.res_cls[idx]);
        TRACE(" tpr_cnt[", idx, "] = ", m_bodyResult.tpr_cnt[idx]);
        TRACE(" pos_x0[", idx, "] = ", m_bodyResult.pos_x0[idx]);
        TRACE(" pos_x1[", idx, "] = ", m_bodyResult.pos_x1[idx]);
        TRACE(" pos_y0[", idx, "] = ", m_bodyResult.pos_y0[idx]);
        TRACE(" pos_y1[", idx, "] = ", m_bodyResult.pos_y1[idx]);
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

    for(int idx = 0; idx < m_bodyResult.res_cnt; idx++)
    {
        TRACE(" res_val[", idx, "] = ", m_bodyResult.res_val[idx]);
        TRACE(" res_cls[", idx, "] = ", m_bodyResult.res_cls[idx]);
        TRACE(" tpr_cnt[", idx, "] = ", m_bodyResult.tpr_cnt[idx]);
        TRACE(" pos_x0[", idx, "] = ", m_bodyResult.pos_x0[idx]);
        TRACE(" pos_x1[", idx, "] = ", m_bodyResult.pos_x1[idx]);
        TRACE(" pos_y0[", idx, "] = ", m_bodyResult.pos_y0[idx]);
        TRACE(" pos_y1[", idx, "] = ", m_bodyResult.pos_y1[idx]);
        TRACE("---------------------------");
    }
}


void
Yolov3Detector::OBD_ComputeResult(uint8_t* obd_lb_addr, uint8_t* obd_mb_addr)
{
	std::vector<BBox> bbox;
    CV_BOX(BoxType::LB_BOX, obd_lb_addr, bbox);
    CV_BOX(BoxType::MB_BOX, obd_mb_addr, bbox);

    TRACE(" num_bbox [", bbox.size(), "] : MAX_BOX[", MAX_BOX, "]");

	// NMS
	int num_nms = 0;
    std::vector<int> res_nms;
	if(bbox.size() <= MAX_BOX)
	{
		res_nms = fw_nms(bbox);
		num_nms = GET_CLIP(res_nms.size(), MAX_BOX);
	}

	Classify_OBD_Result(bbox, num_nms, res_nms);

}

void
Yolov3Detector::CV_BOX(BoxType boxType, uint8_t *obd_addr, std::vector<BBox> &bbox)
{
    std::shared_ptr<Yolov3Param> param = std::dynamic_pointer_cast<Yolov3Param>(m_param);

	int j,i,c,cnt;
	int BX0,BY0,BX1,BY1,CO;
	int BX_partial, BY_partial;
    int box_h, box_w, partial_shift, boxIdx;
    int numBox_before = bbox.size();

    if (boxType == BoxType::LB_BOX) {
        box_h = param->lbbox_h;
        box_w = param->lbbox_w;
        partial_shift = 5;
        boxIdx = 0;
    } else if (boxType == BoxType::MB_BOX) {
        box_h = param->mbbox_h;
        box_w = param->mbbox_w;
        partial_shift = 4;
        boxIdx = 1;
    } else {
        throw std::runtime_error("invalid boxType ");
    }

	cnt = 0;
	for (j = 0; j < box_h; j++)
    {
        BY_partial = j << partial_shift;
        for (i = 0; i < box_w; i++)
        {
            BX_partial = i << partial_shift;
            for (c = 0; c < param->box_size; c++)
            {
                BX0 = cal_pos(BX_partial, LUT_EXP[boxIdx], obd_addr[cnt], 0, param->stride[boxIdx]);
                BY0 = cal_pos(BY_partial, LUT_EXP[boxIdx], obd_addr[cnt+1], 0, param->stride[boxIdx]);
                BX1 = cal_pos(BX_partial, LUT_EXP[boxIdx], obd_addr[cnt+2], 1, param->stride[boxIdx]);
                BY1 = cal_pos(BY_partial, LUT_EXP[boxIdx], obd_addr[cnt+3], 1, param->stride[boxIdx]);

                BX0 = (BX0 > m_modelInfo.width) ? m_modelInfo.width : BX0;
                BY0 = (BY0 > m_modelInfo.height) ? m_modelInfo.height : BY0;
                BX1 = (BX1 > m_modelInfo.width) ? m_modelInfo.width : BX1;
                BY1 = (BY1 > m_modelInfo.height) ? m_modelInfo.height : BY1;

                unsigned short max = 0;
                int max_id = -1;
                for (int n = 0; n < param->class_size; n++)
                {
                    if (obd_addr[cnt+5+n] > max)
                    {
                        max = obd_addr[cnt+5+n];
                        max_id = n;
                    }
                }

                CO = cal_confidence(LUT_CONF[boxIdx],obd_addr[cnt+4],max);
                if( CO > param->thresh_score[1]) /* low threshold */
                {
                    bbox.emplace_back(BX0, BY0, BX1, BY1, CO, max_id);
					//printf("%3d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n", box_idx, BX0, BY0, BX1, BY1, max_id, CO);
                }
                cnt+=15;
            }
            cnt+=3;
        }
    }

    TRACE(__FUNCTION__, ": " , bbox.size() - numBox_before);

}

std::vector<int>
Yolov3Detector::fw_nms(std::vector<BBox> &bbox)
{
    std::shared_ptr<Yolov3Param> param = std::dynamic_pointer_cast<Yolov3Param>(m_param);
    std::vector<int> res_nms;
	// sort
	int num_sort = 0;
	int res_sort[MAX_BOX];

	for (int idx = 0; idx < bbox.size(); idx++)
	{
		int c_idx = idx;
		int c_score = bbox[c_idx].c0;

		for (int idx_sort = 0; idx_sort < num_sort; idx_sort++)
		{
			int t_idx = res_sort[idx_sort];
			int t_score = bbox[t_idx].c0;

			if (c_score > t_score)
			{	// swap
				res_sort[idx_sort] = c_idx;
				c_idx = t_idx;
				c_score = t_score;
			}
		}

		res_sort[num_sort++] = c_idx;
	}


	// area
    std::vector<long int> res_area;
    for (const auto &box : bbox)
    {
		res_area.push_back(std::max(box.xmax - box.xmin, 0.f) * std::max(box.ymax - box.ymin, 0.f));
    }

    std::vector<int> keep_flag(bbox.size(), 1);

	//int num_nms = 0;
	for (int idx_sort = 0; idx_sort < num_sort; idx_sort++)
	{
		int c_idx = res_sort[idx_sort];
		long int c_area = res_area[c_idx];
		int c_score = bbox[c_idx].c0;
		int c_class = bbox[c_idx].c1;

		int th_score_avg = (c_score * 4) >> 3;

		int c_keep = keep_flag[c_idx];
		if(c_keep == 0) continue;

		long int cnt_sim = 0;
		long int sum_x0 = 0;
		long int sum_y0 = 0;
		long int sum_x1 = 0;
		long int sum_y1 = 0;
		for (int idx_nms = idx_sort+1; idx_nms < num_sort; idx_nms++)
		{
			int t_idx = res_sort[idx_nms];
			long int t_area = res_area[t_idx];
			int t_score = bbox[t_idx].c0;
			int t_class = bbox[t_idx].c1;

			int t_keep = keep_flag[t_idx];
			if(t_keep == 0) continue;

			if (c_class == t_class)
			{
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
				if((i_area > th_area_avg) && (t_score > th_score_avg))
				{
					cnt_sim += 1;
					sum_x0 += bbox[t_idx].xmin - bbox[c_idx].xmin;
					sum_y0 += bbox[t_idx].ymin - bbox[c_idx].ymin;
					sum_x1 += bbox[t_idx].xmax - bbox[c_idx].xmax;
					sum_y1 += bbox[t_idx].ymax - bbox[c_idx].ymax;
				}
			}
		}

		if(cnt_sim > 0)
		{
			bbox[c_idx].xmin += sum_x0/cnt_sim;
			bbox[c_idx].ymin += sum_y0/cnt_sim;
			bbox[c_idx].xmax += sum_x1/cnt_sim;
			bbox[c_idx].ymax += sum_y1/cnt_sim;
		}

		if (c_keep)
		{
            res_nms.push_back(c_idx);
		}
	}

    TRACE(__FUNCTION__, ": " , res_nms.size());
    return res_nms;
}

void
Yolov3Detector::Classify_OBD_Result(const std::vector<BBox> &bbox, int num_nms, std::vector<int> &res_nms)
{
	m_obd_result = {0,};
	m_bodyResult = {0,};
	m_faceResult = {0,};

	int idx = 0;
	int selState = 0;
	for (idx = 0; idx < num_nms; idx++)
	{
		int idx_nms = res_nms[idx];
		int _x0 = bbox[idx_nms].xmin;
		int _y0 = bbox[idx_nms].ymin;
		int _x1 = bbox[idx_nms].xmax;
		int _y1 = bbox[idx_nms].ymax;
		int _score = (bbox[idx_nms].c0);
		int _class = (bbox[idx_nms].c1);

		//printf("%2d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n", idx, _x0, _y0, _x1, _y1, _class, _score);

        selState = ((_class == eOBD_PERSON) || (_class == eOBD_FACE)) ? 0 : 1; // objects execept for person, face

		if(selState != 0 && m_obd_result.res_cnt < OBD_RESULT_NUM)
		{
            /* other objects except for person , face */
			m_obd_result.res_cls[m_obd_result.res_cnt]	= _class;
			m_obd_result.pos_x0[m_obd_result.res_cnt] 	= _x0;
			m_obd_result.pos_x1[m_obd_result.res_cnt] 	= _x1;
			m_obd_result.pos_y0[m_obd_result.res_cnt] 	= _y0;
			m_obd_result.pos_y1[m_obd_result.res_cnt] 	= _y1;

			if(_x0 != _x1 && _y0 != _y1)
			{
				m_obd_result.res_val[m_obd_result.res_cnt] = _score;
				m_obd_result.tpr_cnt[m_obd_result.res_cnt] = tcnt_init[_class];
				m_obd_result.res_cnt++;
			}

			//printf("%u:%2d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n", m_obd_result.res_cnt, idx, _x0, _x1, _y0, _y1, _class, _score);
			if (m_obd_result.res_cnt >= OBD_RESULT_NUM) break;
		}
//			else //only for person & face
		{
			if (_class == eOBD_PERSON && m_bodyResult.res_cnt < OBD_RESULT_NUM)
			{
				m_bodyResult.res_cls[m_bodyResult.res_cnt]	= _class;
				m_bodyResult.pos_x0[m_bodyResult.res_cnt] 	= _x0;
				m_bodyResult.pos_x1[m_bodyResult.res_cnt] 	= _x1;
				m_bodyResult.pos_y0[m_bodyResult.res_cnt] 	= _y0;
				m_bodyResult.pos_y1[m_bodyResult.res_cnt] 	= _y1;

				if(_x0 != _x1 && _y0 != _y1)
				{
					m_bodyResult.res_val[m_bodyResult.res_cnt] = _score;
					m_bodyResult.tpr_cnt[m_bodyResult.res_cnt] = tcnt_init[_class];
					m_bodyResult.res_cnt++;
				}
			    //printf("person%u:%2d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n",m_bodyResult.res_cnt, idx, _x0, _x1, _y0, _y1, _class, _score);
			}
			else if(_class == eOBD_FACE && m_faceResult.res_cnt < OBD_RESULT_NUM)
			{
				m_faceResult.res_cls[m_faceResult.res_cnt]	= _class;
				m_faceResult.pos_x0[m_faceResult.res_cnt] 	= _x0;
				m_faceResult.pos_x1[m_faceResult.res_cnt] 	= _x1;
				m_faceResult.pos_y0[m_faceResult.res_cnt] 	= _y0;
				m_faceResult.pos_y1[m_faceResult.res_cnt] 	= _y1;

				if(_x0 != _x1 && _y0 != _y1)
				{
					m_faceResult.res_val[m_faceResult.res_cnt] = _score;
					m_faceResult.tpr_cnt[m_faceResult.res_cnt] = tcnt_init[_class];

					//printf("face%u:%2d: (%4d,%4d,%4d,%4d) [%2d: %5d]\n", m_faceResult.res_cnt, idx, _x0, _x1, _y0, _y1, _class, _score);
					m_faceResult.res_cnt++;
				}
			}

			if(m_bodyResult.res_cnt >= OBD_RESULT_NUM && m_faceResult.res_cnt >= OBD_RESULT_NUM) break;
		}
	}

	if(m_obd_result.res_cnt < 0 || m_obd_result.res_cnt > OBD_RESULT_NUM) m_obd_result.res_cnt = 0;
	if(m_bodyResult.res_cnt < 0 || m_bodyResult.res_cnt > OBD_RESULT_NUM) m_bodyResult.res_cnt = 0;
	if(m_faceResult.res_cnt < 0 || m_faceResult.res_cnt > OBD_RESULT_NUM) m_faceResult.res_cnt = 0;

}



} // end of namespace aif
