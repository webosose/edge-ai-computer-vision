/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/rppg/RppgPostProcessOperation.h>
#include <aif/rppg/RppgInferencePipeDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>
#include <aif/rppg/FiltFilt.h>

#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
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
, m_avgHeartrate(0.0)
, m_signalCondition("Bad")
, m_HRMedArraySize(5)
, m_HRMedArray()
, m_rPPGmHRArraySize(5)
, m_rPPGmHRArray()
, m_countMedHr(0)
, m_cntMedmHR(0)
, m_prevHR(0.0)
, m_rPPGMHr(0.0)
, m_prevrPPGmHR(0.0)
, m_prevFilteredHR(0.0)
, m_avgrPPGmHeartrate(0.0)
, m_hrInfo(0.0)
, m_stableCnt(0)
{
    for(int i = 0; i < m_HRMedArraySize; i++) m_HRMedArray.push_back(0.0f);
    for(int i = 0; i < m_rPPGmHRArraySize; i++) m_rPPGmHRArray.push_back(0.0f);
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
    // memoryDump(outputs.data(), "./swp_output.bin", 400 * sizeof(float));
    // memoryRestore(outputs.data(), "./air_output.bin");
    // std::cout << "OutputTensors: " << std::endl;
    // int cnt=0;
    // for(int i=0; i< 400; i++){
    //     if (cnt<5) std::cout << outputs[i]<<" ";
    //     else {
    //         std::cout << outputs[i] << std::endl;
    //         cnt=0;
    //     }
    //     cnt++;
    // }
    // std::cout << std::endl;

    std::vector<int> output_shape = {static_cast<int>(outputs.size())};
    xt::xarray<float> gg = xt::adapt(outputs.data(), output_shape); // [400]
    xt::xarray<double> double_gg = xt::cast<double>(gg);
    xt::xarray<double> yy;
    bpfFiltFilt(double_gg, yy); // [400]

    std::pair<double, std::string> aeRealtimeHRCalResult = aeRealtimeHRCalAvg.aeRealtimeHRCal(yy, m_fsRe);
    auto hr_info      = aeRealtimeHRCalResult.first;
    m_signalCondition = aeRealtimeHRCalResult.second;
    auto HR_freq = hr_info;
    // auto raw_HR = HR_freq;

    xt::xarray<double> ggg = const_cast<xt::xarray<double>&> (fdescriptor->getFirstInputXarray());
    bpfFiltFilt(ggg, yy);
    std::pair<double, std::string> realtimeHRCalResult = realtimeHRCal.aeRealtimeHRCal(ggg, m_fsRe);
    m_rPPGMHr = realtimeHRCalResult.first;

    int HR_limit = 5;
    if (m_countMedHr < m_HRMedArraySize) m_countMedHr = m_countMedHr + 1;
    m_HRMedArray.pop_front();
    m_HRMedArray.push_back(HR_freq);

    int idx1 = m_HRMedArray.size() - m_countMedHr;
    std::vector<double> p_m_hrMedArray;
    p_m_hrMedArray.insert(p_m_hrMedArray.end(), m_HRMedArray.begin() + idx1, m_HRMedArray.end());
    auto curr_HR = median(p_m_hrMedArray);

    if (m_countMedHr == m_HRMedArraySize) curr_HR = checkHRVarLimit(curr_HR, m_prevHR, HR_limit);
    else curr_HR = HR_freq;

    m_prevHR = curr_HR;

    if (m_cntMedmHR < m_rPPGmHRArraySize) m_cntMedmHR = m_cntMedmHR + 1;
    m_rPPGmHRArray.pop_front();
    m_rPPGmHRArray.push_back(m_rPPGMHr);

    int idx2 = m_rPPGmHRArray.size() - m_cntMedmHR;
    std::vector<double> p_m_rPPGmHRArray;
    p_m_rPPGmHRArray.insert(p_m_rPPGmHRArray.end(), m_rPPGmHRArray.begin() + idx2, m_rPPGmHRArray.end());
    auto curr_rPPG_m_HR = median(p_m_rPPGmHRArray);

    double alpha = 0.0;
    double beta, gamma = 0.0;
    if(m_cntMedmHR == m_rPPGmHRArraySize) {
        curr_rPPG_m_HR = checkHRVarLimit(curr_rPPG_m_HR, m_prevrPPGmHR, HR_limit);
        alpha = 0.5;
        double remained_point = 1.0 - alpha;

        if (m_stableCnt >= 5)
        {
            if (curr_rPPG_m_HR < 110)
            {
                beta  = remained_point * 0.3;
                gamma = remained_point * 0.7;
            }
            else
            {
                beta  = remained_point;
                gamma = 0.0;
            }
        }
        else
        {
            m_stableCnt = m_stableCnt + 1;
            gamma = remained_point;
            beta = 0.0;
        }
    }
    else
    {
        alpha = 0.0;
        gamma = 1.0;
        beta = 0.0;
        curr_rPPG_m_HR = m_rPPGMHr;
    }

    m_prevrPPGmHR = curr_rPPG_m_HR;

    double curr_filtered_HR = m_prevFilteredHR * alpha + curr_rPPG_m_HR * beta + curr_HR * gamma;
    if(m_cntMedmHR == m_rPPGmHRArraySize)
    {
        m_prevFilteredHR = checkHRVarLimit(curr_filtered_HR, m_prevFilteredHR, HR_limit);
    }
    else
    {
        m_prevFilteredHR = curr_filtered_HR;
    }

    curr_HR = m_prevFilteredHR;
    m_avgHeartrate = curr_HR;
    m_avgrPPGmHeartrate = curr_rPPG_m_HR;
    m_hrInfo = m_avgHeartrate;
    // memoryDump(&m_hrInfo, "./swp_hr_result.bin", sizeof(float));

    fdescriptor->addRppgFinalResult(m_hrInfo, m_signalCondition);
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

double RppgPostProcessOperation::checkHRVarLimit(double curr_HR, double prev_HR, int limit_th)
{
    double diff_HR = prev_HR - curr_HR;
    if(diff_HR > limit_th) curr_HR = prev_HR - limit_th;
    else if(diff_HR < -limit_th) curr_HR = prev_HR + limit_th;
    return curr_HR;
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

double RppgPostProcessOperation::median(std::vector<double> &v)
{
    double ans = 0.0;
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