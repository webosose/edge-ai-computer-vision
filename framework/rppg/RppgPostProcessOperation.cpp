/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/rppg/RppgPostProcessOperation.h>
#include <aif/rppg/RppgInferencePipeDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>
#include <aif/rppg/FiltFilt.h>
#include <aif/rppg/FftLib.h>

#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <xtensor/xsort.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xbuilder.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xmath.hpp>

namespace aif {

RppgPostProcessOperation::RppgPostProcessOperation(const std::string& id)
: BridgeOperation(id)
, m_fsRe(50)
, m_hr(0.0)
, m_countHr(0)
, m_countHr2(0)
, m_avgHeartrate(0.0)
, m_heartrateArraySize(7)
, m_heartrateArray()
, m_heartrateArraySize2(10)
, m_heartrateArray2()
, m_signalCondition("Bad")
{
    for(int i = 0; i < m_heartrateArraySize; i++) m_heartrateArray.push_back(0.0f);
    for(int i = 0; i < m_heartrateArraySize2; i++) m_heartrateArray2.push_back(0.0f);
}

RppgPostProcessOperation::~RppgPostProcessOperation()
{
}

bool RppgPostProcessOperation::runImpl(const std::shared_ptr<NodeInput>& input)
{
    auto& descriptor = input->getDescriptor();
    if (!descriptor) {
        Loge(m_id, ": failed to run operaiton (descriptor is null)");
        return false;
    }
    std::shared_ptr<RppgInferencePipeDescriptor> fdescriptor=
        std::dynamic_pointer_cast<RppgInferencePipeDescriptor>(input->getDescriptor());

    std::vector<float> outputs = const_cast<std::vector<float>&> (fdescriptor->getRppgOutputs());
    // const auto& outputs =fdescriptor->getRppgOutputs();
    int out_batch = fdescriptor->getBatchSize();
    int out_channel = fdescriptor->getChannelSize();
    // convert vector to xtensor array
    std::vector<int> output_shape = {out_batch, out_channel};
    xt::xarray<float> gg = xt::adapt(outputs.data(), out_batch * out_channel, xt::no_ownership(), output_shape); // [400]
    xt::xarray<double> double_gg = xt::cast<double>(gg);
    double_gg = double_gg.reshape({out_channel});

    xt::xarray<double> yy;
    bpfFiltFilt(double_gg, yy); // [400]

    auto HR_info = aeRealtimeHRCal(yy);
    std::cout << "checking Before HR: " << HR_info << std::endl;
    std::cout << "checking heartrateArray Size: " << m_heartrateArray.size() << std::endl;

    for(int i=0; i<m_heartrateArray.size(); i++) std::cout << m_heartrateArray[i] << std::endl;

    if (m_countHr < m_heartrateArraySize) m_countHr += 1;

    m_heartrateArray.pop_front();
    m_heartrateArray.push_back(HR_info);

    if(m_countHr > 1) {
        int HR_variation_th = 5;
        int startIdx = m_heartrateArraySize - m_countHr;
        std::vector<float> p_m_heartrateArray;
        p_m_heartrateArray.insert(p_m_heartrateArray.end(), m_heartrateArray.begin() + startIdx, m_heartrateArray.end()); // 확인해야함
        auto currHR = median(p_m_heartrateArray);

        float diffHR = m_avgHeartrate - currHR;
        std::cout << "checking diffHR: " << diffHR << std::endl;
        if (diffHR > HR_variation_th) {
            std::cout << "limit step to 5 BPM" << std::endl;
            m_avgHeartrate -= HR_variation_th;
        }
        else if (diffHR < -HR_variation_th) {
            std::cout << "limit step to -5 BPM" << std::endl;
            m_avgHeartrate += HR_variation_th;
        }
        else m_avgHeartrate = currHR;
    }
    else m_avgHeartrate = m_heartrateArray.back();

    if (m_countHr2 < m_heartrateArraySize2) m_countHr2 += 1;

    m_heartrateArray2.pop_front();
    m_heartrateArray2.push_back(m_avgHeartrate);

    float sumHeartrateArray2 = 0.0;
    int startIdx2 = m_heartrateArraySize2 - m_countHr2;
    for(auto i = m_heartrateArray2.begin() + startIdx2; i < m_heartrateArray2.end(); ++i){
        sumHeartrateArray2 += (*i);
    }
    m_avgHeartrate = sumHeartrateArray2 / m_countHr2;

    HR_info = m_avgHeartrate + 0.5;
    // std::cout << "Try this" << std::endl;
    // memoryDump(&HR_info, "./HR_result_3m.bin", sizeof(float));
    // std::cout << "END" << std::endl;

    fdescriptor->addRppgFinalResult(HR_info, m_signalCondition);
    return true;
}

int RppgPostProcessOperation::getButterBPFParam(xt::xarray<double>& b, xt::xarray<double>& a, uint8_t nOption)
{
    // Code from AI Research Center
    const uint8_t nMaxOptionNum = 4;
    //option 0 : order 3, band 0.3~2 Hz
    //option 1 : order 3, band 0.5~3 Hz
    //option 2 : order 3, band 0.5~4 Hz
    //option 3 : order 3, band 0.5~5 Hz

    const xt::xarray<double> b_opt = {{ 0.00099517, 0., -0.00298552, 0., 0.00298552, 0., -0.00099517},
                                    { 0.00289819, 0., -0.00869458, 0., 0.00869458, 0., -0.00289819},
                                    {0.00716767, 0., -0.021503, 0., 0.021503, 0., -0.00716767},
                                    { 0.01382007, 0., -0.04146022, 0., 0.04146022, 0., -0.01382007}};

    const xt::xarray<double> a_opt = {{ 1., -5.54704419, 12.85541954, -15.93478169, 11.14327437, -4.16862754, 0.6517602},
                                    { 1., -5.3100446, 11.82233454, -14.13367487,9.57288223, -3.48356279, 0.53207537},
                                    { 1., -5.04456768, 10.694677, -12.21737356, 7.94145633, -2.78601016, 0.41183913},
                                    { 1., -4.78290878, 9.63126604, -10.49397778, 6.54436315, -2.21613085, 0.31742548}};

    if (nOption >= nMaxOptionNum) { // out of option_range
        return 1;
    }

    b = xt::row(b_opt, nOption);
    a = xt::row(a_opt, nOption);

    return 0;
}

int RppgPostProcessOperation::bpfFiltFilt(xt::xarray<double>& data, xt::xarray<double>& filtered_sig)
{
    // Code from AI Research Center
    xt::xarray<double> a, b;
    kb::math::FilterCoefficients<double> fc;
    std::vector<double> vec_signal(data.begin(), data.end());

    if (getButterBPFParam(b, a)) Loge(__func__,"Error: out of option_range");

    fc.m_CoefficientsA.resize(a.size(), 0);
    fc.m_CoefficientsB.resize(a.size(), 0);

    fc.m_CoefficientsA.assign(a.begin(), a.end());
    fc.m_CoefficientsB.assign(b.begin(), b.end());

    kb::math::FiltFilt<double> filtfilt(fc);
    auto zeroPhaseFiltered = filtfilt.ZeroPhaseFiltering(vec_signal);
    filtered_sig = xt::zeros<double>({zeroPhaseFiltered.size()});
    std::copy(zeroPhaseFiltered.begin(), zeroPhaseFiltered.end(), filtered_sig.begin());

    return 0;
}

float RppgPostProcessOperation::aeRealtimeHRCal(xt::xarray<double>& sigArray)
{
    xt::xarray<double> data_filtered = sigArray;
    // bpfFiltFilt(sigArray, data_filtered); // [400]
    int N = 1024;
    float T = 1.0f / m_fsRe;
    m_signalCondition = "Bad";

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

    auto heartRate = freqPk * 60.0;

    if (m_hr == 0.0) m_hr = heartRate;
    else m_hr = 0.5 * m_hr + 0.5 * heartRate;
    m_hr = float(m_hr);
    m_signalCondition = "Normal";

    return m_hr;
}

xt::xarray<double> RppgPostProcessOperation::hannCal(int len)
{
    xt::xarray<double> hannOutput = xt::zeros<double>({len});
    for (int i = 0; i < len; i++) hannOutput.at(i) = 0.5 - 0.5 * cos((2 * M_PI * i) / (len - 1));

    return hannOutput;
}

xt::xarray<double> RppgPostProcessOperation::rfftFreqCal(int n, float d)
{
    double val = 1.0 / (n * d); // 0.0488281
    auto N = ((n + (2 - 1)) / 2) + 1; // 513
    xt::xarray<double> results = xt::arange(0, N);
    return results * val;
}

int RppgPostProcessOperation::rfft1024(xt::xarray<double>& pInput_buff, xt::xarray<std::complex<double>>& pOutput_buff)
{
    // Code from AI Research Center
    const int N = 1024;
    uint8_t u8ifftFlag = 0; //forward transform
    uint8_t u8bitReverseFlag = 1; // Enables bit reversal of output
    float32_t buff[N * 2];
    std::fill(buff, buff + N * 2, 0); //reset buffer

    for (int i = 0; i < N; i++) buff[2 * i] = pInput_buff(i);

    arm_cfft_f32(&arm_cfft_sR_f32_len1024, (float *)buff, u8ifftFlag, u8bitReverseFlag);

    for (int i = 0; i < N / 2 + 1; i++) pOutput_buff(i) = std::complex<double>(buff[2 * i], buff[2 * i + 1]);

    return 0;
}

float RppgPostProcessOperation::median(std::vector<float> &v)
{
    float ans = 0.0;
    std::sort(v.begin(), v.end());
    int n = v.size();

    if(n % 2 == 1){
        ans = v[static_cast<double>(n) / 2];
    }
    else {
        int i = static_cast<double>(n) / 2;
        ans = (v[i - 1] + v[i]) / 2 ;
    }
    return ans;
}

} // end of namespace aif
