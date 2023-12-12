/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_RPPG_POST_PROCESS_OPERATION_H
#define AIF_RPPG_POST_PROCESS_OPERATION_H

#include <aif/pipe/BridgeOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>
#include <aif/rppg/AeRealtimeHRCalAvg.h>


#include <xtensor/xarray.hpp>
#include <xtensor/xcomplex.hpp>
#include <vector>

namespace aif {

class RppgPostProcessOperation : public BridgeOperation {
    private:
        RppgPostProcessOperation(const std::string& id);

    public:
        virtual ~RppgPostProcessOperation();
        template<typename T1, typename T2>
            friend class NodeOperationFactoryRegistration;

        bool runImpl(const std::shared_ptr<NodeInput>& input) override;
    protected:
        int getButterBPFParam(xt::xarray<double>& b, xt::xarray<double>& a, uint8_t nOption = 1);
        double checkHRVarLimit(double curr_HR, double prev_HR, int limit_th);
        int bpfFiltFilt(xt::xarray<double>& data, xt::xarray<double>& filtered_sig);
        double fftArrayAvg(double freq);
        double median(std::vector<double> &v);
    private:
        std::vector<float> rppgOutputs;
        int m_fsRe;
        float m_avgHeartrate;
        std::string m_signalCondition;
        int m_HRMedArraySize;
        std::deque<double> m_HRMedArray;
        int m_rPPGmHRArraySize;
        std::deque<double> m_rPPGmHRArray;
        int m_countMedHr;
        int m_cntMedmHR;
        double m_prevHR;
        double m_rPPGMHr;
        double m_prevrPPGmHR;
        double m_prevFilteredHR;
        double m_avgrPPGmHeartrate;
        float m_hrInfo;
        AeRealtimeHRCalAvg aeRealtimeHRCalAvg;
        AeRealtimeHRCalAvg realtimeHRCal;
};

NodeOperationFactoryRegistration<RppgPostProcessOperation, BridgeOperationConfig>
    rppg_post_process("rppg_post_process");

} // end of namespace aif

#endif // AIF_RPPG_POST_PROCESS_OPERATION_H