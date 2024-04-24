/*
* Copyright (c) 2023 LG Electronics Inc.
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef AIF_RPPG_DETECTOR_H
#define AIF_RPPG_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/rppg/RppgDescriptor.h>
#include <aif/rppg/RppgParam.h>
#include <xtensor/xarray.hpp>
#include <xtensor/xcomplex.hpp>

namespace aif {

class RppgDetector : public Detector {
    public:
        RppgDetector(const std::string& modelPath);
        virtual ~RppgDetector();

    protected:
        std::shared_ptr<DetectorParam> createParam() override;
        void setModelInfo(TfLiteTensor* inputTensor) override;
        t_aif_status preProcessing() override;
};

} // end of namespace aif

#endif // AIF_RPPG_DETECTOR_H
