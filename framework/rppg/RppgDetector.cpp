/*
* Copyright (c) 2023 LG Electronics Inc.
* SPDX-License-Identifier: Apache-2.0
*/

#include <aif/rppg/RppgDetector.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>
#include <aif/rppg/CubicSpline.h>
#include <aif/rppg/FiltFilt.h>
#include <aif/rppg/FftLib.h>

#include <cmath>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <typeinfo>
#include <xtensor/xsort.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xbuilder.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xcomplex.hpp>


namespace aif {

RppgDetector::RppgDetector(const std::string& modelPath)
: Detector(modelPath)
, m_fsRe(50)
, m_targetTime(8)
, m_winSec(1.6f)
, m_hr(0.0)
, m_alpha(0.5)
, m_beta(0.5)
, m_signalCondition("Bad")
{
}

RppgDetector::~RppgDetector()
{
}

std::shared_ptr<DetectorParam> RppgDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<RppgParam>();
    return param;
}

void RppgDetector::setModelInfo(TfLiteTensor* inputTensor)
{
    if (inputTensor == nullptr) {
        Loge("inputTensor ptr is null");
        return;
    }
    m_modelInfo.batchSize = 1; //inputTensor->dims->data[0];
    m_modelInfo.height = inputTensor->dims->data[1]; // 400
    m_modelInfo.width = inputTensor->dims->data[2]; // 2

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
}

t_aif_status RppgDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv matrix!!");
        }

        int batchSize = m_modelInfo.batchSize; // 1
        int height = m_modelInfo.height; // 400
        int width = m_modelInfo.width; // 2

        if (m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter not initialized!!");
        }

        // Pre-Processing - Using XTENSOR
        std::vector<int> shape = {img.rows, img.cols}; // rows=120, cols=4
        xt::xarray<double> mesh_dataset = xt::adapt((double*)img.data, img.total() * img.channels(), xt::no_ownership(), shape); // total=480, channels=1
        xt::xarray<double> timeInfo = xt::cumsum(xt::view(mesh_dataset, xt::all(), 0)); // [120]
        xt::xarray<double> intrTime = xt::arange(timeInfo[0], timeInfo[timeInfo.size() - 1], (1.0f / m_fsRe)); // [397]

        // CubicSpline Interpolation - AI Reseach Center
        xt::xarray<double> R_yNew, G_yNew, B_yNew;
        CubicSpline cubicSpline;
        for (int i = 0; i < 3; i++) {
            xt::xarray<double> RGB = xt::view(mesh_dataset, xt::all(), i+1); // [120]
            cubicSpline.CSpline( timeInfo.reshape({timeInfo.size()}), RGB.reshape({RGB.size()}) );

            if (i == 0) R_yNew = cubicSpline.CalcCSpline(intrTime); // [397]
            else if (i == 1)  G_yNew = cubicSpline.CalcCSpline(intrTime); // [397]
            else B_yNew = cubicSpline.CalcCSpline(intrTime); // [397]
        }
        auto xt_rbgInter = xt::vstack(xt::xtuple(R_yNew, G_yNew, B_yNew)); // [3, 397]
        auto rgbInter = xt::transpose(xt_rbgInter); // [397, 3]

        int st_idx = -(m_targetTime * m_fsRe); // -400
        auto p_rgbInter = xt::view(rgbInter, xt::range(st_idx, xt::placeholders::_), xt::all()); // [397, 3]
        auto p_intrTime = xt::view(intrTime, xt::range(st_idx, xt::placeholders::_)); // [397]

        int N = static_cast<int>(p_intrTime.shape()[0]); // [397]
        xt::xarray<double> rPPGmDat = xt::zeros<double>({N}); // [397] == H
        int l = (int)std::ceil(m_winSec * m_fsRe); // 80
        // auto C = xt::zeros<double>({l, 3}); // [80, 3]
        xt::xarray<double> P = {{0.0, 1.0, -1.0}, {-2.0, 1.0, 1.0}}; // [2, 3]

        for (int n = 1; n < N; n++) {
            int m = n - l + 1;
            if (m > 0) {
                xt::xarray<double> p_p_rgbInter = xt::view(p_rgbInter, xt::range(m, n), xt::all());
                xt::xarray<double> m_p_rgbInter = xt::mean(p_p_rgbInter, {0});
                xt::xarray<double> Cn = xt::transpose(p_p_rgbInter / m_p_rgbInter); // [3, 79]
                auto S = dotProduct(P, Cn); // [2, 79]

                xt::xarray<double> S_first  = xt::view(S, 0, xt::all());
                xt::xarray<double> S_second = xt::view(S, 1, xt::all());
                double std_s_first  = xt::stddev(S_first)();
                double std_s_second = xt::stddev(S_second)();

                xt::xarray<double> h_ = xt::zeros<double>({S_first.shape()[0]});
                for(int i = 0; i < S_first.shape()[0]; i++) {
                    h_.at(i) = S_first.at(i) + (std_s_first / std_s_second) * S_second.at(i);
                }

                double h_mean = xt::mean(h_, {0})();
                int count = 0;
                for(int i = m; i < n; i++) {
                    double h_update = h_.at(count++) - h_mean;
                    rPPGmDat.at(i) = rPPGmDat.at(i) + h_update;
                }

            }
        }

        xt::xarray<double> greenDat = xt::view(p_rgbInter, xt::all(), 1); // [397]
        // Normalization
        double rPPGmDat_min = xt::amin(rPPGmDat)();
        double rPPGmDat_max = xt::amax(rPPGmDat)();
        double greenDat_min = xt::amin(greenDat)();
        double greenDat_max = xt::amax(greenDat)();
        xt::xarray<double> data11 = (rPPGmDat - rPPGmDat_min) / (rPPGmDat_max - rPPGmDat_min); // [397]
        xt::xarray<double> data21 = (greenDat - greenDat_min) / (greenDat_max - greenDat_min); // [397]

        int lenList = (int)(m_targetTime * m_fsRe - rPPGmDat.size()); // FOR NOW 3
        xt::xarray<double> fillList;
        xt::xarray<double> inputt = {0.5};
        for(int i = 0; i < lenList; i++) {
            if (i == 0) fillList = inputt;
            else fillList = xt::concatenate(xt::xtuple(fillList, inputt)); // append 0.5
        }

        xt::xarray<double> data_1_1 = xt::concatenate(xt::xtuple(data11, fillList), 0); // [400]
        xt::xarray<double> data_2_1 = xt::concatenate(xt::xtuple(data21, fillList), 0); // [400]
        xt::xarray<double> inputDat = xt::stack(xt::xtuple(data_1_1, data_2_1), 1); // [400, 2]
        xt::xarray<double> reshapeData = inputDat.reshape({1, inputDat.shape()[0], 2 }); // [1, 400, 2]
        xt::xarray<float> double_reshapeData = xt::cast<float>(reshapeData);

        // OPTION 1) convert xarray -> cv::mat
        // cv::Mat cv_reshapeData3(double_reshapeData.shape()[0], double_reshapeData.shape()[1], CV_32FC2, double_reshapeData.data());

        float* inputTensor = m_interpreter->typed_input_tensor<float>(0);
        // std::memcpy(inputTensor, cv_reshapeData3.ptr<float>(0), batchSize * height  * width * sizeof(float)); // Option1) memcpy cvMat -> input tensor
        std::memcpy(inputTensor, double_reshapeData.data(), batchSize * height  * width * sizeof(float)); // Option2) memcpy xarray -> input tensor

        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status RppgDetector::preProcessing()
{
    return kAifOk;
}

t_aif_status RppgDetector::postProcessing(const cv::Mat &img, std::shared_ptr<Descriptor> &descriptor)
{
    try {
        const std::vector<int> &tensor_outputs = m_interpreter->outputs();
        TfLiteTensor *output = m_interpreter->tensor(tensor_outputs[0]); // [1, 400]
        if (output == nullptr) {
            throw std::runtime_error("can't get tflite tensor_output!!");
        }

        int out_batch = output->dims->data[0]; // 1
        int out_channel = output->dims->data[1]; //400

        float* data= reinterpret_cast<float*>(output->data.data);
        // convert to xtensor array
        std::vector<int> output_shape = {out_batch, out_channel};
        xt::xarray<float> gg = xt::adapt(data, out_batch * out_channel, xt::no_ownership(), output_shape); // [400]
        xt::xarray<double> double_gg = xt::cast<double>(gg);
        double_gg = double_gg.reshape({out_channel});

        xt::xarray<double> yy;
        bpfFiltFilt(double_gg, yy); // [400]

        auto HR_info = aeRealtimeHRCal(yy); // Final BPM

        std::shared_ptr<RppgDescriptor> rppgDescriptor = std::dynamic_pointer_cast<RppgDescriptor>(descriptor);
        if (rppgDescriptor != nullptr) rppgDescriptor->addRppgOutput(HR_info, m_signalCondition);

    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }

    return kAifOk;
}

xt::xarray<double> RppgDetector::dotProduct(xt::xarray<double>& p, xt::xarray<double>& cn)
{
    xt::xarray<double> xt_output = xt::zeros<double>({p.shape()[0], cn.shape()[1]}); // [2, 79] | zero initialization
    // matrix x matrix dot product
    for (int i = 0; i < p.shape()[0]; i++) { // 2
        for(int j = 0; j < cn.shape()[1]; j++) { // 79
            for( int k = 0; k < p.shape()[1]; k++) { // 3
                xt_output(i, j) = xt_output(i, j) + p(i, k) * cn(k, j);
            }
        }
    }

    return xt_output;
}

int RppgDetector::getButterBPFParam(xt::xarray<double>& b, xt::xarray<double>& a, uint8_t nOption)
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

int RppgDetector::bpfFiltFilt(xt::xarray<double>& data, xt::xarray<double>& filtered_sig)
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

float RppgDetector::aeRealtimeHRCal(xt::xarray<double>& sigArray)
{
    xt::xarray<double> data_filtered;
    bpfFiltFilt(sigArray, data_filtered); // [400]
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
    xt::xarray<std::complex<double>> yf = xt::zeros<std::complex<double>>({513});
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

xt::xarray<double> RppgDetector::hannCal(int len)
{
    xt::xarray<double> hannOutput = xt::zeros<double>({len});
    for (int i = 0; i < len; i++) hannOutput.at(i) = 0.5 - 0.5 * cos((2 * M_PI * i) / (len - 1));

    return hannOutput;
}

xt::xarray<double> RppgDetector::rfftFreqCal(int n, float d)
{
    double val = 1.0 / (n * d); // 0.0488281
    auto N = ((n + (2 - 1)) / 2) + 1; // 513
    xt::xarray<double> results = xt::arange(0, N);
    return results * val;
}

int RppgDetector::rfft1024(xt::xarray<double>& pInput_buff, xt::xarray<std::complex<double>>& pOutput_buff)
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

} // end of namespace aif