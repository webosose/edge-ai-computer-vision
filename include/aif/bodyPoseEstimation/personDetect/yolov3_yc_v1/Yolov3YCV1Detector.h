/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV3_YC_V1_DETECTOR_H
#define AIF_YOLOV3_YC_V1_DETECTOR_H

#include <aif/bodyPoseEstimation/personDetect/PersonDetectDetector.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_yc_v1/Yolov3YCV1Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_yc_v1/Yolov3YCV1Param.h>

#define OBD_SCORE_MAX 65535 // 0xFF
#define OBD_RESULT_NUM 10
#define SZ_COORD		(5 + OBD_RESULT_NUM) // 15
#define MAX_BOX 200 //(SZ_LBBOX + SZ_MBBOX) // 405+1620 = 2025
#define tcnt_init_PERSON 8
#define tcnt_init_FACE 8
#define tcnt_init_PET 8
#define tcnt_init_OBJ 8

#define SZ_BOX			3
#define SZ_LBBOX_W		15
#define SZ_LBBOX_H		9
#define SZ_MBBOX_W		30
#define SZ_MBBOX_H		18

#define SZ_CONV_LBBOX	(SZ_LBBOX_W * SZ_LBBOX_H * SZ_BOX * SZ_COORD)
#define SZ_CONV_MBBOX	(SZ_MBBOX_W * SZ_MBBOX_H * SZ_BOX * SZ_COORD)

#define GET_CLIP(_x,_y)			((((_x) > (_y)) ? (_y) : (_x)))
#define YUV_CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)


namespace aif {

typedef enum
{
	eOBD_PERSON		= 0,
	eOBD_FACE		= 1,
	eOBD_CAR		= 2,
	eOBD_BUS		= 3,
	eOBD_MOTOCYCLE	= 4,
	eOBD_BICYCLE	= 5,
	eOBD_TRAIN		= 6,
	eOBD_CAT		= 7,
	eOBD_DOG		= 8,
	eOBD_TV			= 9,
    eOBD_MAX        = 10,
} t_obd_class_type;

typedef struct {
    unsigned int box_show;				//0x0
    unsigned int all_show_mode;			//0x4
    int res_cnt;					//0x8
    int sel_cls; // 0~9				//0xC
    int res_val[OBD_RESULT_NUM];	//0x10 ~ 0x34
    int res_cls[OBD_RESULT_NUM];	//0x38 ~ 0x5C
    int pos_x0 [OBD_RESULT_NUM];	//0x60 ~ 0x84
    int pos_x1 [OBD_RESULT_NUM];	//0x88 ~ 0xAC
    int pos_y0 [OBD_RESULT_NUM];	//0xB0 ~ 0xD4
    int pos_y1 [OBD_RESULT_NUM];	//0xD8 ~ 0xFC
    int tpr_cnt[OBD_RESULT_NUM];	//0x100 ~ 0x124
} t_pqe_obd_result;


class Yolov3YCV1Detector : public PersonDetector
{
public:
    enum class BoxType {
        LB_BOX = 0,
        MB_BOX = 1,
    };

    Yolov3YCV1Detector(const std::string& modelPath);
    virtual ~Yolov3YCV1Detector();


protected:
    std::shared_ptr<DetectorParam> createParam() override;
    void setModelInfo(TfLiteTensor* inputTensor) override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override;

private:
    static inline unsigned int cal_pos(int partial,
                                       const unsigned int LUT[],
                                       unsigned short par,
                                       bool cal_mode,
                                       unsigned int  box_size)
    {
        int tmp_result;
        if(cal_mode)
            tmp_result =  partial + (box_size>>1) + LUT[par];
        else
            tmp_result =  partial + (box_size>>1) - LUT[par];
        if(tmp_result < 0)
            return 0;
        else
            return tmp_result;
    }

    static inline int cal_confidence(const unsigned int LUT[],
                                     unsigned short par,
                                     int max_pro)
    {
        return (LUT[par])*(LUT[max_pro]);
    }

    void OBD_ComputeResult(unsigned short* obd_lb_addr, unsigned short* obd_mb_addr);
    void transform2OriginCoord(const cv::Mat& img, t_pqe_obd_result &result);
    void CV_BOX(BoxType boxType, unsigned short* obd_addr, std::vector<BBox> &bbox);
    std::vector<int> fw_nms(std::vector<BBox> &bbox);
    void Classify_OBD_Result(const std::vector<BBox> &bbox, int num_nms, std::vector<int> &res_nms);
    void getPaddedImage(const cv::Mat& src, cv::Mat& dst);
    void FaceMatching();
    bool checkUpdate(int index, const BBox &currBbox);
    void RD_Result(unsigned short *obd_lb, unsigned int* addr,unsigned int size_x, unsigned int size_y);


private:

    t_pqe_obd_result	m_obd_result;
    t_pqe_obd_result	m_bodyResult;
    t_pqe_obd_result	m_faceResult;
    float m_ImgResizingScale;
    cv::Size m_paddedSize;
    int m_leftBorder;
    int m_topBorder;
    bool m_IsBodyDetect; // Body or Face ?
    std::vector<std::pair<BBox, float>> m_prevBboxList; // for previous frame detection results
    std::vector<std::pair<std::string, BBox>> m_GTBBoxes; // for debug, ground truth bbox
    int m_frameId;

    static constexpr int tcnt_init[eOBD_MAX] = {tcnt_init_PERSON, tcnt_init_FACE, tcnt_init_PET, tcnt_init_PET,
                                      tcnt_init_OBJ, tcnt_init_OBJ, tcnt_init_OBJ, tcnt_init_OBJ,
                                      tcnt_init_OBJ, tcnt_init_OBJ};

    const unsigned int LUT_CONF[2][256] = {
        // LUT_LB_CONF[256]
        {   0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     1,     1,     1,     2,     3,     3,     5,
            6,     8,    10,    13,    17,    22,    28,    35,    44,    54,    66,    80,    95,   111,   128,   144,
            160,   175,   189,   201,   211,   220,   227,   233,   238,   242,   245,   247,   249,   250,   252,   252,
            253,   254,   254,   254,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
            255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255
        },
        // LUT_MB_CONF[256]
        {   0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     1,     1,     1,     1,     1,     2,     2,     3,     3,     4,     4,     5,     6,
            7,     8,     9,    11,    12,    14,    16,    19,    22,    25,    28,    32,    37,    42,    47,    53,
            60,    67,    74,    82,    91,   100,   109,   118,   128,   137,   146,   155,   164,   173,   181,   188,
            195,   202,   208,   213,   218,   223,   227,   230,   233,   236,   239,   241,   243,   244,   246,   247,
            248,   249,   250,   251,   251,   252,   252,   253,   253,   254,   254,   254,   254,   254,   255,   255
        }
    };

    const unsigned int LUT_EXP[2][256] = {
        // LUT_LB_EXP
        {   0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     1,     1,     1,     2,     3,     3,     5,     6,     8,    11,    14,    18,    24,    32,    41,
            53,    69,    90,   117,   152,   198,   257,   334,   434,   563,   731,   949,  1232,  1599,  2075,  2694,
            3496,  4538,  5890,  7645,  9923, 12880, 16718, 21699, 28163, 36554, 47445, 61581, 79927, 103741, 134648, 174764,
            226832, 294413, 382127, 495975, 643742, 835532, 1084464, 1407559, 1826915, 2371210, 3077667, 3994599, 5184715, 6729403, 8734301, 11336521
        },
        // LUT_MB_EXP
        {   0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     1,     1,     1,     1,     2,     2,     2,     3,     3,     4,
            4,     5,     6,     7,     8,    10,    11,    13,    16,    18,    21,    24,    28,    33,    38,    44,
            51,    60,    69,    80,    93,   108,   125,   145,   168,   195,   226,   262,   304,   352,   408,   473,
            548,   635,   736,   853,   988,  1145,  1327,  1538,  1782,  2065,  2392,  2772,  3212,  3722,  4312,  4997
        }
    };
};

} // end of namespace aif

#endif // AIF_YOLOV3_YC_V1_DETECTOR_H
