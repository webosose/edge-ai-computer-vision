/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_RPPG_PRE_PROCESS_OPERATION_H
#define AIF_RPPG_PRE_PROCESS_OPERATION_H

#include <aif/pipe/BridgeOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>

#include <xtensor/xarray.hpp>
#include <xtensor/xcomplex.hpp>
#include <vector>

namespace aif {

class RppgPreProcessOperation : public BridgeOperation {
    private:
        RppgPreProcessOperation(const std::string& id);

    public:
        virtual ~RppgPreProcessOperation();
        template<typename T1, typename T2>
            friend class NodeOperationFactoryRegistration;

        bool runImpl(const std::shared_ptr<NodeInput>& input) override;
    protected:
        xt::xarray<double> dotProduct(xt::xarray<double>& p, xt::xarray<double>& cn);
    private:
        std::vector<float> rppgOutputs;
        int m_fsRe;
        int m_targetTime;
        float m_winSec;
};
NodeOperationFactoryRegistration<RppgPreProcessOperation, BridgeOperationConfig>
    rppg_pre_process("rppg_pre_process");

} // end of namespace aif

#endif // AIF_RPPG_PRE_PROCESS_OPERATION_H