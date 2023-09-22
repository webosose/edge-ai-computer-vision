/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV3_V1_DETECTOR_H
#define AIF_YOLOV3_V1_DETECTOR_H

#include <aif/bodyPoseEstimation/personDetect/PersonDetectDetector.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_v1/Yolov3V1Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_v1/Yolov3V1Param.h>

#define OBD_SCORE_MAX    65535 // 0xFF
#define OBD_RESULT_NUM   10
#define MAX_BOX          200 //(SZ_LBBOX + SZ_MBBOX) // 405+1620 = 2025
#define tcnt_init_PERSON 8
#define tcnt_init_FACE   8
#define tcnt_init_PET    8
#define tcnt_init_OBJ    8
#define SZ_BOX           3
#define SZ_BOX_SB        1
#define SZ_LBBOX_W       15
#define SZ_LBBOX_H       9 //18
#define SZ_MBBOX_W       30
#define SZ_MBBOX_H       18
#define SZ_SBBOX_W       60
#define SZ_SBBOX_H       36
#define SZ_CONV_LB_BOX   (SZ_LBBOX_W * SZ_LBBOX_H * SZ_BOX * 4)
#define SZ_CONV_LB_CONF  (SZ_LBBOX_W * SZ_LBBOX_H * SZ_BOX * (1 + OBD_RESULT_NUM))
#define SZ_CONV_MB_BOX   (SZ_MBBOX_W * SZ_MBBOX_H * SZ_BOX * 4)
#define SZ_CONV_MB_CONF  (SZ_MBBOX_W * SZ_MBBOX_H * SZ_BOX * (1 + OBD_RESULT_NUM))
#define SZ_CONV_SB_BOX   (SZ_SBBOX_W * SZ_SBBOX_H * SZ_BOX_SB * 4)
#define SZ_CONV_SB_CONF  (SZ_SBBOX_W * SZ_SBBOX_H * SZ_BOX_SB * (1 + OBD_RESULT_NUM))

#define GET_CLIP(_x,_y)  ((((_x) > (_y)) ? (_y) : (_x)))


namespace aif {

typedef enum
{
    eOBD_PERSON     = 0,
    eOBD_FACE       = 1,
    eOBD_CAR        = 2,
    eOBD_BUS        = 3,
    eOBD_MOTOCYCLE  = 4,
    eOBD_BICYCLE    = 5,
    eOBD_TRAIN      = 6,
    eOBD_CAT        = 7,
    eOBD_DOG        = 8,
    eOBD_TV         = 9,
    eOBD_MAX        = 10,
} t_obd_class_type;

typedef struct {
    unsigned int box_show;       //0x0
    unsigned int all_show_mode;  //0x4
    int res_cnt;                 //0x8
    int sel_cls; // 0~9          //0xC
    int res_val[OBD_RESULT_NUM]; //0x10 ~ 0x34
    int res_cls[OBD_RESULT_NUM]; //0x38 ~ 0x5C
    int pos_x0 [OBD_RESULT_NUM]; //0x60 ~ 0x84
    int pos_x1 [OBD_RESULT_NUM]; //0x88 ~ 0xAC
    int pos_y0 [OBD_RESULT_NUM]; //0xB0 ~ 0xD4
    int pos_y1 [OBD_RESULT_NUM]; //0xD8 ~ 0xFC
    int tpr_cnt[OBD_RESULT_NUM]; //0x100 ~ 0x124
} t_pqe_obd_result;


class Yolov3V1Detector : public PersonDetector
{
public:
    enum class BoxType {
        LB_BOX = 0,
        MB_BOX = 1,
        SB_BOX = 2,
    };

    Yolov3V1Detector(const std::string& modelPath);
    virtual ~Yolov3V1Detector();


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
        if(cal_mode) tmp_result =  partial + (box_size >> 1) + LUT[par];
        else tmp_result =  partial + (box_size >> 1) - LUT[par];

        if(tmp_result < 0) return 0;
        else return tmp_result;
    }

    static inline int cal_confidence(const unsigned int LUT[],
                                    unsigned short par,
                                    int max_pro)
    {
        return (LUT[par]) * (LUT[max_pro]);
    }

    void OBD_ComputeResult(std::vector<unsigned short>& obd_lb_box_addr, std::vector<unsigned short>& obd_lb_conf_addr,
                        std::vector<unsigned short>& obd_mb_box_addr, std::vector<unsigned short>& obd_mb_conf_addr,
                        std::vector<unsigned short>& obd_sb_box_addr, std::vector<unsigned short>& obd_sb_conf_addr);
    void transform2OriginCoord(const cv::Mat& img, t_pqe_obd_result &result);
    void CV_BOX(BoxType boxType, std::vector<unsigned short>& obd_addr, std::vector<unsigned short>& obd_conf, std::vector<BBox> &bbox);
    std::vector<int> fw_nms(std::vector<BBox> &bbox);
    void Classify_OBD_Result(const std::vector<BBox> &bbox, int num_nms, std::vector<int> &res_nms);
    void getPaddedImage(const cv::Mat& src, cv::Mat& dst);
    void FaceMatching();
    bool checkUpdate(int index, const BBox &currBbox);
    void RD_Result_box(std::vector<unsigned short>& obd_box, unsigned int* addr,unsigned int size_x, unsigned int size_y);
    void RD_Result_conf(std::vector<unsigned short>& obd_conf,unsigned int* addr,unsigned int size_x, unsigned int size_y);
    void RD_Result_box_SB(std::vector<unsigned short>& obd_box, unsigned int* addr,unsigned int size_x, unsigned int size_y);
    void RD_Result_conf_SB(std::vector<unsigned short>& obd_conf,unsigned int* addr,unsigned int size_x, unsigned int size_y);

private:
    t_pqe_obd_result    m_obd_result;
    t_pqe_obd_result    m_bodyResult;
    t_pqe_obd_result    m_faceResult;
    float m_ImgResizingScale;
    cv::Size m_paddedSize;
    int m_leftBorder;
    int m_topBorder;
    bool m_IsBodyDetect; // Body or Face ?
    std::vector<std::pair<BBox, float>> m_prevBboxList; // for previous frame detection results
    std::vector<std::pair<std::string, BBox>> m_GTBBoxes; // for debug, ground truth bbox
    int m_frameId;

    static constexpr int tcnt_init[eOBD_MAX] = {tcnt_init_PERSON, tcnt_init_FACE, tcnt_init_PET, tcnt_init_PET,
                                      tcnt_init_OBJ, tcnt_init_OBJ, tcnt_init_OBJ, tcnt_init_OBJ, tcnt_init_OBJ, tcnt_init_OBJ};
    const unsigned int LUT_CONF[3][256] = {
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
            0,     0,     0,     0,     1,     1,     1,     1,     1,     1,     2,     2,     2,     2,     3,     3,
            4,     5,     6,     7,     8,     9,    11,    12,    15,    17,    20,    23,    27,    31,    36,    41,
            47,    54,    61,    69,    78,    87,    97,   107,   117,   128,   138,   148,   158,   168,   177,   186,
            194,   201,   208,   214,   219,   224,   228,   232,   235,   238,   240,   243,   244,   246,   247,   248,
            249,   250,   251,   252,   252,   253,   253,   253,   253,   254,   254,   254,   254,   254,   254,   255,
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
            0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
            0,     0,     0,     0,     0,     0,     1,     1,     1,     1,     1,     1,     2,     2,     3,     3,
            4,     5,     6,     7,     8,    10,    12,    15,    18,    21,    26,    31,    36,    43,    51,    59,
            69,    79,    90,   102,   115,   128,   140,   153,   165,   176,   186,   196,   204,   212,   219,   224,
            229,   234,   237,   240,   243,   245,   247,   248,   249,   250,   251,   252,   252,   253,   253,   254
        },
        // LUT_SB_CONF[256]
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
            0,     0,     0,     1,     1,     1,     1,     1,     2,     2,     2,     3,     4,     4,     5,     7,
            8,    10,    12,    14,    17,    21,    25,    30,    36,    42,    50,    58,    68,    79,    90,   102,
            115,   128,   140,   153,   165,   176,   187,   197,   205,   213,   219,   225,   230,   234,   238,   241,
            243,   245,   247,   248,   250,   251,   251,   252,   253,   253,   253,   254,   254,   254,   254,   254
        }
    };

    const unsigned int LUT_EXP[3][256] = {
        // LUT_LB_EXP
        {   6,     6,     6,     6,     6,     6,     6,     7,     7,     7,     7,     7,     7,     8,     8,     8,
            8,     8,     9,     9,     9,     9,     9,    10,    10,    10,    10,    11,    11,    11,    11,    12,
            12,    12,    12,    13,    13,    13,    14,    14,    14,    15,    15,    15,    16,    16,    17,    17,
            17,    18,    18,    19,    19,    19,    20,    20,    21,    21,    22,    22,    23,    24,    24,    25,
            25,    26,    26,    27,    28,    28,    29,    30,    31,    31,    32,    33,    34,    34,    35,    36,
            37,    38,    39,    40,    41,    41,    42,    43,    45,    46,    47,    48,    49,    50,    51,    53,
            54,    55,    56,    58,    59,    61,    62,    63,    65,    67,    68,    70,    71,    73,    75,    77,
            78,    80,    82,    84,    86,    88,    90,    93,    95,    97,    99,   102,   104,   107,   109,   112,
            115,   117,   120,   123,   126,   129,   132,   135,   138,   142,   145,   148,   152,   156,   159,   163,
            167,   171,   175,   179,   184,   188,   193,   197,   202,   207,   212,   217,   222,   227,   233,   238,
            244,   250,   256,   262,   268,   274,   281,   288,   294,   302,   309,   316,   324,   331,   339,   347,
            356,   364,   373,   382,   391,   400,   410,   420,   430,   440,   450,   461,   472,   483,   495,   507,
            519,   531,   544,   557,   570,   584,   598,   612,   627,   642,   657,   673,   689,   705,   722,   740,
            757,   775,   794,   813,   832,   852,   872,   893,   915,   937,   959,   982,  1005,  1029,  1054,  1079,
            1105,  1131,  1158,  1186,  1214,  1243,  1273,  1303,  1335,  1366,  1399,  1432,  1467,  1502,  1538,  1574,
            1612,  1651,  1690,  1730,  1772,  1814,  1857,  1902,  1947,  1994,  2041,  2090,  2140,  2191,  2243,  2297
        },
        // LUT_MB_EXP
        {   5,     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,     6,     6,     6,     6,
            6,     6,     6,     6,     6,     6,     6,     7,     7,     7,     7,     7,     7,     7,     7,     7,
            8,     8,     8,     8,     8,     8,     8,     8,     8,     9,     9,     9,     9,     9,     9,     9,
            10,    10,    10,    10,    10,    10,    10,    11,    11,    11,    11,    11,    11,    12,    12,    12,
            12,    12,    13,    13,    13,    13,    13,    14,    14,    14,    14,    14,    15,    15,    15,    15,
            16,    16,    16,    16,    16,    17,    17,    17,    18,    18,    18,    18,    19,    19,    19,    19,
            20,    20,    20,    21,    21,    21,    22,    22,    22,    23,    23,    23,    24,    24,    24,    25,
            25,    26,    26,    26,    27,    27,    28,    28,    28,    29,    29,    30,    30,    31,    31,    32,
            32,    33,    33,    34,    34,    35,    35,    36,    36,    37,    37,    38,    38,    39,    40,    40,
            41,    41,    42,    43,    43,    44,    45,    45,    46,    47,    47,    48,    49,    50,    50,    51,
            52,    53,    54,    54,    55,    56,    57,    58,    59,    60,    60,    61,    62,    63,    64,    65,
            66,    67,    68,    69,    70,    71,    72,    74,    75,    76,    77,    78,    79,    81,    82,    83,
            84,    86,    87,    88,    90,    91,    92,    94,    95,    97,    98,   100,   101,   103,   104,   106,
            107,   109,   111,   112,   114,   116,   118,   119,   121,   123,   125,   127,   129,   131,   133,   135,
            137,   139,   141,   143,   145,   147,   150,   152,   154,   157,   159,   161,   164,   166,   169,   172,
            174,   177,   179,   182,   185,   188,   191,   194,   197,   199,   203,   206,   209,   212,   215,   218
        },
        // LUT_SB_EXP
        {   1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
            1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
            1,     1,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
            2,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
            2,     2,     2,     2,     2,     2,     2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
            3,     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
            3,     3,     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
            4,     4,     4,     4,     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
            5,     5,     5,     6,     6,     6,     6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
            7,     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,     8,     8,     8,     8,     8,
            8,     8,     8,     8,     9,     9,     9,     9,     9,     9,     9,     9,    10,    10,    10,    10,
            10,    10,    10,    10,    11,    11,    11,    11,    11,    11,    11,    12,    12,    12,    12,    12,
            12,    13,    13,    13,    13,    13,    13,    14,    14,    14,    14,    14,    15,    15,    15,    15,
            15,    16,    16,    16,    16,    16,    17,    17,    17,    17,    18,    18,    18,    18,    19,    19,
            19,    19,    20,    20,    20,    20,    21,    21,    21,    22,    22,    22,    22,    23,    23,    23,
            24,    24,    24,    25,    25,    25,    26,    26,    26,    27,    27,    27,    28,    28,    28,    29
        }
    };
};

} // end of namespace aif

#endif // AIF_YOLOV3_V1_DETECTOR_H
