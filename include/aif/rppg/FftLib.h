/*
* Copyright (c) 2023 LG Electronics Inc.
* SPDX-License-Identifier: Apache-2.0
*/
// Code from AI Research Center

typedef float float32_t;

#define ARMBITREVINDEXTABLE_1024_TABLE_LENGTH ((uint16_t)1800)

typedef struct
{
    uint16_t fftLen;             /**< length of the FFT. */
    const float32_t *pTwiddle;   /**< points to the Twiddle factor table. */
    uint16_t *pBitRevTable;      /**< points to the bit reversal table. */
    uint16_t bitRevLength;       /**< bit reversal table length. */
} arm_cfft_instance_f32;


extern const float32_t twiddleCoef_1024[2048];
extern uint16_t armBitRevIndexTable1024[ARMBITREVINDEXTABLE_1024_TABLE_LENGTH];

extern const arm_cfft_instance_f32 arm_cfft_sR_f32_len1024;

void arm_radix8_butterfly_f32(float32_t * pSrc, uint16_t fftLen, const float32_t * pCoef, uint16_t twidCoefModifier);
void arm_cfft_radix8by4_f32( arm_cfft_instance_f32 * S, float32_t * p1);
void arm_cfft_radix8by2_f32( arm_cfft_instance_f32 * S, float32_t * p1);
void arm_bitreversal_32 (uint32_t * pSrc, const uint16_t bitRevLen, const uint16_t * pBitRevTable);
void arm_cfft_f32( const arm_cfft_instance_f32 * S, float32_t * p1, uint8_t ifftFlag, uint8_t bitReverseFlag);





