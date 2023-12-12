/*
* Copyright (c) 2023 LG Electronics Inc.
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef AIF_AE_REALTIMEHR_CAL_AVG_H
#define AIF_AE_REALTIMEHR_CAL_AVG_H

#include <xtensor/xarray.hpp>

#include <deque>

namespace aif {

class AeRealtimeHRCalAvg
{
public:
    AeRealtimeHRCalAvg();
    virtual ~AeRealtimeHRCalAvg();
private:
   std::pair<double, std::string> fftArrayAvg(double freq, std::string signalCondition);
public:
    std::pair<double, std::string> aeRealtimeHRCal(xt::xarray<double>& sigArray, int fs_re);
private:
   xt::xarray<double> hannCal(int len);
   xt::xarray<double> rfftFreqCal(int n, float d);
   int rfft1024(xt::xarray<double>& pInput_buff, xt::xarray<std::complex<double>>& pOutput_buff);
   double median(std::vector<double> &v);
private:
    double m_HR;
    int m_count;
    int m_prevCount;
    int m_countAll;
    int m_winSize;
    std::deque<double> m_freqArray;
};

} // end of namespace aif

#endif // AIF_AE_REALTIMEHR_CAL_AVG_H