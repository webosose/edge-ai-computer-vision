/*
* Copyright (c) 2023 LG Electronics Inc.
* SPDX-License-Identifier: Apache-2.0
*/
// Code from AI Research Center

#include <aif/rppg/AeRealtimeHRCalAvg.h>
#include <aif/rppg/FftLib.h>

#include <xtensor/xio.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xsort.hpp>

namespace aif {

AeRealtimeHRCalAvg::AeRealtimeHRCalAvg()
: m_HR(0.0)
, m_count(0)
, m_prevCount(0)
, m_countAll(0)
, m_winSize(5)
, m_freqArray()
{
    for(int i = 0; i < m_winSize; i++) m_freqArray.push_back(0.0f);
}

AeRealtimeHRCalAvg::~AeRealtimeHRCalAvg()
{
}

std::pair<double, std::string> AeRealtimeHRCalAvg::fftArrayAvg(double freq, std::string signalCondition)
{
    m_countAll = m_countAll + 1;
    signalCondition = "Normal";

    if (m_count <= m_winSize) {
        m_freqArray.pop_front();
        m_freqArray.push_back(freq);
        m_count = m_count + 1;
    } else {
        double sum = 0.0;
        for(int i=0; i< m_freqArray.size(); i++)
        {
            sum += m_freqArray[i];
        }

        double mean_m_freqArray = sum / m_freqArray.size();

        if (std::abs(mean_m_freqArray - freq) < 0.2)
        {
            m_freqArray.pop_front();
            m_freqArray.push_back(freq);
            m_prevCount = m_countAll;
            signalCondition = "Good";
        }
        else
        {
            signalCondition = "Normal";
            m_freqArray.pop_front();
            double sum_2 = 0.0;
            for(int i = 0; i < m_freqArray.size() - 1; i++) {
                sum_2 += m_freqArray[i];
            }
            double mean_m_freqArray2 = sum_2 / (m_freqArray.size()- 1.0);
            m_freqArray.push_back(mean_m_freqArray2);
            auto error_count = m_countAll - m_prevCount;

            if (error_count > 3) {
                signalCondition = "Bad";
                m_count = 2;
            }
        }
    }

    int idx = m_freqArray.size() - m_count;
    std::vector<double> p_m_freqArray;
    p_m_freqArray.insert(p_m_freqArray.end(), m_freqArray.begin() + idx, m_freqArray.end());
    auto avg_freq = median(p_m_freqArray);

    return std::make_pair(avg_freq, signalCondition);
}

std::pair<double, std::string> AeRealtimeHRCalAvg::aeRealtimeHRCal(xt::xarray<double>& sigArray, int fs_re)
{
    xt::xarray<double> data_filtered = sigArray;
    int N = 1024;
    float T = 1.0f / fs_re;
    std::string signalCondition = "Bad";

    int sizeOf_dataFiltered = static_cast<int>(data_filtered.size());
    auto window = hannCal(sizeOf_dataFiltered); // [400]

    data_filtered = data_filtered * window;

    xt::xarray<double> Input_buff =  xt::zeros<double>({1024});
    for(int i=0; i< data_filtered.size(); i++) {
        Input_buff.at(i) = data_filtered.at(i);
    }
    xt::xarray<std::complex<double>> yf = xt::zeros<std::complex<double>>({513}); // output_buff
    rfft1024(Input_buff, yf); // [400]
    auto yf_ab = abs(yf);
    xt::xarray<double> frequency = rfftFreqCal(N, T); //[513]
    auto idx = xt::where((frequency > 0.8) && (frequency < 3)); // [1, 45] | Frequency range (0.8~3Hz(48~180bpm))

    auto xf_re = xt::index_view(frequency, idx[0]); // [45]
    auto y_re = xt::index_view(yf_ab, idx[0]);

    auto freqMaxIdx = xt::argmax(y_re);
    double freqPk = xt::index_view(xf_re, freqMaxIdx)();

    std::pair<double, std::string> fftArrayAvgResult = fftArrayAvg(freqPk, signalCondition);
    double avg_freq = fftArrayAvgResult.first;
    signalCondition = fftArrayAvgResult.second;

    auto heartRate = avg_freq * 60.0;
    m_HR = heartRate; // self.HR or avg_Heartrate

    return std::make_pair(m_HR, signalCondition);
}

xt::xarray<double> AeRealtimeHRCalAvg::hannCal(int len)
{
    xt::xarray<double> hannOutput = xt::zeros<double>({len});
    for (int i = 0; i < len; i++)
    {
        hannOutput.at(i) = 0.5 - 0.5 * cos((2 * M_PI * i) / (len - 1));
    }
    return hannOutput;
}

xt::xarray<double> AeRealtimeHRCalAvg::rfftFreqCal(int n, float d)
{
    double val = 1.0 / (n * d); // 0.0488281
    auto N = ((n + (2 - 1)) / 2) + 1; // 513
    xt::xarray<double> results = xt::arange(0, N);
    return results * val;
}

int AeRealtimeHRCalAvg::rfft1024(xt::xarray<double>& pInput_buff, xt::xarray<std::complex<double>>& pOutput_buff)
{
    // Code from AI Research Center
    const int N = 1024;
    uint8_t u8ifftFlag = 0; //forward transform
    uint8_t u8bitReverseFlag = 1; // Enables bit reversal of output
    float32_t buff[N * 2];
    std::fill(buff, buff + N * 2, 0); //reset buffer

    for (int i = 0; i < N; i++)
    {
        buff[2 * i] = pInput_buff(i);
    }

    arm_cfft_f32(&arm_cfft_sR_f32_len1024, (float *)buff, u8ifftFlag, u8bitReverseFlag);

    for (int i = 0; i < N / 2 + 1; i++)
    {
        pOutput_buff(i) = std::complex<double>(buff[2 * i], buff[2 * i + 1]);
    }
    return 0;
}

double AeRealtimeHRCalAvg::median(std::vector<double> &v)
{
    double ans = 0.0;
    std::sort(v.begin(), v.end());
    int n = v.size();

    if(n % 2 == 1)
    {
        ans = v[static_cast<double>(n) / 2];
    }
    else
    {
        int i = static_cast<double>(n) / 2;
        ans = (v[i - 1] + v[i]) / 2 ;
    }
    return ans;
}

} // end of namespace aif
