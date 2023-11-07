/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_RPPG_POST_PROCESS_OPERATION_H
#define AIF_RPPG_POST_PROCESS_OPERATION_H

#include <aif/pipe/BridgeOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>

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
        int bpfFiltFilt(xt::xarray<double>& data, xt::xarray<double>& filtered_sig);
        float aeRealtimeHRCal(xt::xarray<double>& sigArray);
        xt::xarray<double> hannCal(int len);
        xt::xarray<double> rfftFreqCal(int n, float d);
        int rfft1024(xt::xarray<double>& pInput_buff, xt::xarray<std::complex<double>>& pOutput_buff);
    private:
        std::vector<float> rppgOutputs;
        int m_fsRe;
        double m_hr;
        std::string m_signalCondition;
};

NodeOperationFactoryRegistration<RppgPostProcessOperation, BridgeOperationConfig>
    rppg_post_process("rppg_post_process");

} // end of namespace aif

#endif // AIF_RPPG_POST_PROCESS_OPERATION_H