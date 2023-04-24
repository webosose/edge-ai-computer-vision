/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV3_V2_DETECTOR_H
#define AIF_YOLOV3_V2_DETECTOR_H

#include <aif/bodyPoseEstimation/personDetect/PersonDetectDetector.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_v2/Yolov3V2Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_v2/Yolov3V2Param.h>

#define OBD_SCORE_MAX 65535 // 0xFF
#define OBD_RESULT_NUM 10
#define MAX_BOX 200 //(SZ_LBBOX + SZ_MBBOX) // 405+1620 = 2025
#define tcnt_init_PERSON 8
#define tcnt_init_FACE 8
#define tcnt_init_PET 8
#define tcnt_init_OBJ 8
#define SZ_BOX			3
#define SZ_LBBOX_W		15
#define SZ_LBBOX_H		9 //18
#define SZ_MBBOX_W		30
#define SZ_MBBOX_H		18
#define SZ_CONV_LB_BOX	(SZ_LBBOX_W * SZ_LBBOX_H * SZ_BOX * 4)
#define SZ_CONV_LB_CONF	(SZ_LBBOX_W * SZ_LBBOX_H * SZ_BOX * (1 + OBD_RESULT_NUM))
#define SZ_CONV_MB_BOX	(SZ_MBBOX_W * SZ_MBBOX_H * SZ_BOX * 4)
#define SZ_CONV_MB_CONF	(SZ_MBBOX_W * SZ_MBBOX_H * SZ_BOX * (1 + OBD_RESULT_NUM))

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
    unsigned int box_show;			//0x0
    unsigned int all_show_mode;	    //0x4
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


class Yolov3V2Detector : public PersonDetector
{
public:
    enum class BoxType {
        LB_BOX = 0,
        MB_BOX = 1,
    };

    Yolov3V2Detector(const std::string& modelPath);
    virtual ~Yolov3V2Detector();


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

    void OBD_ComputeResult(unsigned short* obd_lb_box_addr, unsigned short* obd_lb_conf_addr, unsigned short* obd_mb_box_addr, unsigned short* obd_mb_conf_addr);
    void transform2OriginCoord(const cv::Mat& img, t_pqe_obd_result &result);
    void CV_BOX(BoxType boxType, unsigned short *obd_addr, unsigned short *obd_conf, std::vector<BBox> &bbox);
    std::vector<int> fw_nms(std::vector<BBox> &bbox);
    void Classify_OBD_Result(const std::vector<BBox> &bbox, int num_nms, std::vector<int> &res_nms);
    void getPaddedImage(const cv::Mat& src, cv::Mat& dst);
    bool checkUpdate(int index, const BBox &currBbox);
    void RD_Result_box(unsigned short *obd_box, unsigned int* addr,unsigned int size_x, unsigned int size_y);
    void RD_Result_conf(unsigned short *obd_conf,unsigned int* addr,unsigned int size_x, unsigned int size_y);

private:
    t_pqe_obd_result	m_obd_result;
    t_pqe_obd_result	m_bodyResult;
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
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
            1,     1,     2,     2,     3,     4,     5,     6,     7,     9,    12,    15,    18,    23,    28,    35,
            42,    51,    61,    72,    85,    99,   113,   128,   142,   156,   170,   183,   194,   204,   213,   220,
            227,   232,   237,   240,   243,   246,   248,   249,   250,   251,   252,   253,   253,   254,   254,   254
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
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     1,     1,     1,     2,     2,
            3,     3,     4,     5,     6,     7,     9,    11,    13,    16,    19,    23,    27,    32,    38,    45,
            53,    61,    71,    81,    92,   103,   115,   128,   140,   152,   163,   174,   184,   194,   202,   210,
            217,   223,   228,   232,   236,   239,   242,   244,   246,   248,   249,   250,   251,   252,   252,   253,
            253,   254,   254,   254,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255
        }
    };

    const unsigned int LUT_EXP[2][256] = {
        // LUT_LB_EXP
        {   13,    14,    14,    15,    16,    16,    17,    18,    18,    19,    20,    21,    21,    22,    23,    24,
            25,    26,    27,    28,    29,    30,    32,    33,    34,    35,    37,    38,    40,    41,    43,    45,
            46,    48,    50,    52,    54,    56,    58,    61,    63,    65,    68,    71,    73,    76,    79,    82,
            85,    89,    92,    96,   100,   104,   108,   112,   116,   121,   125,   130,   135,   140,   146,   152,
            158,   164,   170,   177,   183,   191,   198,   206,   214,   222,   231,   240,   249,   259,   269,   279,
            290,   301,   313,   325,   338,   351,   364,   378,   393,   408,   424,   441,   458,   475,   494,   513,
            533,   554,   575,   597,   621,   645,   670,   696,   723,   751,   780,   810,   841,   874,   908,   943,
            980,  1018,  1057,  1098,  1141,  1185,  1231,  1279,  1328,  1380,  1433,  1489,  1546,  1606,  1669,  1733,
            1800,  1870,  1943,  2018,  2096,  2177,  2262,  2350,  2441,  2535,  2634,  2736,  2842,  2952,  3066,  3185,
            3308,  3437,  3570,  3708,  3852,  4001,  4156,  4318,  4485,  4659,  4839,  5027,  5222,  5424,  5634,  5853,
            6079,  6315,  6560,  6814,  7078,  7352,  7637,  7933,  8241,  8560,  8892,  9237,  9595,  9966, 10353, 10754,
            11171, 11604, 12053, 12521, 13006, 13510, 14033, 14577, 15142, 15729, 16339, 16972, 17630, 18313, 19023, 19760,
            20526, 21321, 22147, 23006, 23897, 24823, 25785, 26785, 27823, 28901, 30021, 31184, 32393, 33648, 34952, 36307,
            37714, 39175, 40694, 42271, 43909, 45611, 47378, 49214, 51122, 53103, 55161, 57298, 59519, 61826, 64222, 66710,
            69296, 71981, 74771, 77669, 80679, 83805, 87053, 90427, 93931, 97571, 101353, 105280, 109360, 113599, 118001, 122574,
            127324, 132259, 137384, 142708, 148239, 153984, 159951, 166150, 172589, 179277, 186225, 193442, 200939, 208726, 216815, 225217
        },
        // LUT_MB_EXP
        {    6,     6,     6,     6,     6,     6,     6,     6,     6,     6,     6,     6,     6,     7,     7,     7,
            7,     7,     7,     7,     7,     7,     7,     7,     8,     8,     8,     8,     8,     8,     8,     8,
            8,     8,     9,     9,     9,     9,     9,     9,     9,     9,     9,    10,    10,    10,    10,    10,
            10,    10,    10,    11,    11,    11,    11,    11,    11,    11,    12,    12,    12,    12,    12,    12,
            12,    13,    13,    13,    13,    13,    13,    14,    14,    14,    14,    14,    14,    15,    15,    15,
            15,    15,    16,    16,    16,    16,    16,    16,    17,    17,    17,    17,    18,    18,    18,    18,
            18,    19,    19,    19,    19,    20,    20,    20,    20,    21,    21,    21,    21,    22,    22,    22,
            22,    23,    23,    23,    23,    24,    24,    24,    25,    25,    25,    26,    26,    26,    26,    27,
            27,    27,    28,    28,    28,    29,    29,    29,    30,    30,    31,    31,    31,    32,    32,    32,
            33,    33,    34,    34,    34,    35,    35,    36,    36,    37,    37,    38,    38,    38,    39,    39,
            40,    40,    41,    41,    42,    42,    43,    43,    44,    44,    45,    45,    46,    47,    47,    48,
            48,    49,    49,    50,    51,    51,    52,    52,    53,    54,    54,    55,    56,    56,    57,    58,
            58,    59,    60,    61,    61,    62,    63,    64,    64,    65,    66,    67,    67,    68,    69,    70,
            71,    72,    72,    73,    74,    75,    76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
            86,    87,    88,    89,    90,    91,    92,    93,    94,    95,    96,    98,    99,   100,   101,   102,
            104,   105,   106,   107,   109,   110,   111,   113,   114,   115,   117,   118,   120,   121,   122,   124
        }
    };
};

} // end of namespace aif

#endif // AIF_YOLOV3_V2_DETECTOR_H
