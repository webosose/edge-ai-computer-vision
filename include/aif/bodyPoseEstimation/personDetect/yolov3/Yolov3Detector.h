/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV3_DETECTOR_H
#define AIF_YOLOV3_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Param.h>

#define OBD_RESULT_NUM 10
#define MAX_BOX 200 //(SZ_LBBOX + SZ_MBBOX) // 405+1620 = 2025
#define tcnt_init_PERSON 8
#define tcnt_init_FACE 8
#define tcnt_init_PET 8
#define tcnt_init_OBJ 8

#define GET_CLIP(_x,_y)			((((_x) > (_y)) ? (_y) : (_x)))


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


class Yolov3Detector : public Detector
{
public:
    enum class BoxType {
        LB_BOX = 0,
        MB_BOX = 1,
    };

    Yolov3Detector(const std::string& modelPath);
    virtual ~Yolov3Detector();


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
                                       uint8_t par,
                                       unsigned int cal_mode,
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
                                     uint8_t par,
                                     int max_pro)
    {
        return (LUT[par])*(LUT[max_pro]);
    }

    void OBD_ComputeResult(uint8_t* obd_lb_addr, uint8_t* obd_mb_addr);
    void transform2OriginCoord(const cv::Mat& img, t_pqe_obd_result &result);
    void CV_BOX(BoxType boxType, uint8_t *obd_addr, std::vector<BBox> &bbox);
    std::vector<int> fw_nms(std::vector<BBox> &bbox);
    void Classify_OBD_Result(const std::vector<BBox> &bbox, int num_nms, std::vector<int> &res_nms);
    void getPaddedImage(const cv::Mat& src, cv::Mat& dst);
    void FaceMatching();

private:

    t_pqe_obd_result	m_obd_result;
    t_pqe_obd_result	m_bodyResult;
    t_pqe_obd_result	m_faceResult;
    float m_ImgResizingScale;
    cv::Size m_paddedSize;
    int m_leftBorder;
    int m_topBorder;
    bool m_IsBodyDetect; // Body or Face ?


    static constexpr int tcnt_init[eOBD_MAX] = {tcnt_init_PERSON, tcnt_init_FACE, tcnt_init_OBJ, tcnt_init_OBJ,
                                      tcnt_init_OBJ, tcnt_init_OBJ, tcnt_init_OBJ, tcnt_init_PET,
                                      tcnt_init_PET, tcnt_init_OBJ};

    const unsigned int LUT_CONF[2][256] = {
        // LUT_LB_CONF[256]
        {    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         1,     1,     1,     1,     1,     1,     1,     2,     2,     2,     2,     3,     3,     3,     4,     4,
	         5,     5,     6,     6,     7,     8,     8,     9,    10,    11,    12,    14,    15,    17,    18,    20,
	         22,    24,    26,    29,    31,    34,    37,    40,    44,    47,    51,    56,    60,    65,    69,    74,
	         80,    85,    91,    97,   103,   109,   115,   121,   128,   134,   140,   146,   152,   158,   164,   170,
	         175,   181,   186,   190,   195,   199,   204,   208,   211,   215,   218,   221,   224,   226,   229,   231,
	         233,   235,   237,   238,   240,   241,   243,   244,   245,   246,   247,   247,   248,   249,   249,   250,
	         250,   251,   251,   252,   252,   252,   253,   253,   253,   253,   254,   254,   254,   254,   254,   254,
	         254,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255
        },
        // LUT_MB_CONF[256]
        {    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	         0,     0,     0,     0,     1,     1,     1,     1,     1,     1,     1,     2,     2,     2,     2,     3,
	         3,     3,     4,     4,     4,     5,     5,     6,     7,     7,     8,     9,    10,    11,    12,    13,
	         15,    16,    17,    19,    21,    23,    25,    27,    30,    32,    35,    38,    42,    45,    49,    53,
	         57,    61,    66,    71,    76,    81,    86,    92,    97,   103,   109,   115,   121,   128,   134,   140,
	         146,   152,   158,   163,   169,   174,   179,   184,   189,   194,   198,   202,   206,   210,   213,   217,
	         220,   223,   225,   228,   230,   232,   234,   236,   238,   239,   240,   242,   243,   244,   245,   246,
	         247,   248,   248,   249,   250,   250,   251,   251,   251,   252,   252,   252,   253,   253,   253,   253
        }
    };

    const unsigned int LUT_EXP[2][256] = {
        // LUT_LB_EXP
        {    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
             0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
             0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
             0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
             0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
             0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
             0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
             0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
             0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
             0,     0,     0,     0,     0,     1,     1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
             3,     3,     3,     4,     4,     4,     5,     6,     6,     7,     8,     8,     9,    10,    12,    13,
             14,    16,    17,    19,    21,    23,    26,    29,    32,    35,    38,    42,    47,    52,    57,    63,
             70,    77,    85,    93,   103,   114,   125,   138,   153,   168,   186,   205,   226,   249,   275,   304,
             335,   369,   407,   449,   496,   547,   603,   665,   733,   809,   892,   984,  1085,  1197,  1320,  1456,
             1605,  1770,  1953,  2153,  2375,  2619,  2889,  3186,  3514,  3875,  4274,  4713,  5198,  5732,  6322,  6972,
             7689,  8480,  9352, 10314, 11375, 12545, 13835, 15258, 16827, 18558, 20467, 22572, 24893, 27453, 30276, 33390
        },
        // LUT_MB_EXP
        {    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
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
	         0,     1,     1,     1,     1,     1,     1,     1,     2,     2,     2,     2,     3,     3,     3,     4,
	         4,     5,     5,     6,     6,     7,     8,     9,     9,    10,    12,    13,    14,    16,    17,    19,
	         21,    23,    25,    28,    31,    34,    37,    41,    45,    50,    55,    61,    67,    73,    81,    89,
	         98,   108,   119,   131,   144,   158,   174,   192,   211,   233,   256,   282,   310,   341,   375,   413,
	         455,   500,   551,   606,   667,   734,   808,   889,   978,  1076,  1184,  1303,  1434,  1578,  1737,  1911
        }
    };
};

} // end of namespace aif

#endif // AIF_YOLOV3_DETECTOR_H
