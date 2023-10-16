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

        // std::shared_ptr<DetectorParam> createParam() override;

    protected:
        std::shared_ptr<DetectorParam> createParam() override;
        void setModelInfo(TfLiteTensor* inputTensor) override;
        t_aif_status fillInputTensor(const cv::Mat& img) override;
        t_aif_status preProcessing() override;
        t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;
        xt::xarray<double> dotProduct(xt::xarray<double>& p, xt::xarray<double>& cn);
        int getButterBPFParam(xt::xarray<double>& b, xt::xarray<double>& a, uint8_t nOption = 1);
        int bpfFiltFilt(xt::xarray<double>& data, xt::xarray<double>& filtered_sig);
        float aeRealtimeHRCal(xt::xarray<double>& sigArray);
        xt::xarray<double> hannCal(int len);
        xt::xarray<double> rfftFreqCal(int n, float d);
        int rfft1024(xt::xarray<double>& pInput_buff, xt::xarray<std::complex<double>>& pOutput_buff);

    private:
        int m_fsRe;
        int m_targetTime;
        float m_winSec;
        double m_hr;
        double m_alpha;
        double m_beta;
        std::string m_signalCondition;
};

} // end of namespace aif

#endif // AIF_RPPG_DETECTOR_H
