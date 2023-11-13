/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/rppg/RppgPreProcessOperation.h>
#include <aif/rppg/RppgInferencePipeDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>
#include <aif/rppg/CubicSpline.h>

#include <aif/rppg/FiltFilt.h>
#include <aif/rppg/FftLib.h>

#include <cmath>
#include <iostream>
#include <vector>
#include <xtensor/xsort.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xbuilder.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xmath.hpp>

namespace aif {

RppgPreProcessOperation::RppgPreProcessOperation(const std::string& id)
: BridgeOperation(id)
, m_fsRe(50)
, m_targetTime(8)
, m_winSec(1.6f)
{
}

RppgPreProcessOperation::~RppgPreProcessOperation()
{
}

bool RppgPreProcessOperation::runImpl(const std::shared_ptr<NodeInput>& input)
{
    auto& descriptor = input->getDescriptor();
    if (!descriptor) {
        Loge(m_id, ": failed to run operaiton (descriptor is null)");
        return false;
    }
    std::shared_ptr<RppgInferencePipeDescriptor> fdescriptor=
        std::dynamic_pointer_cast<RppgInferencePipeDescriptor>(input->getDescriptor());

    const cv::Mat& image = descriptor->getImage();
    if (image.empty()) {
        Loge("failed to get Image");
        return false;
    }

    // std::cout << "Try this" << std::endl;
    // memoryRestore(image.data, "/usr/share/aif/example/face_irgb_3m.bin");
    // std::vector<int> shape = {120, 4}; // rows=120, cols=4
    // xt::xarray<double> mesh_dataset = xt::adapt(image.data, 480, xt::no_ownership(), shape); // total=480, channels=1
    // std::cout << "END" << std::endl;

    // Pre-Processing - Using XTENSOR
    std::vector<int> shape = {image.rows, image.cols}; // rows=120, cols=4
    xt::xarray<double> mesh_dataset = xt::adapt((double*)image.data, image.total() * image.channels(), xt::no_ownership(), shape); // total=480, channels=1
    xt::xarray<double> timeInfo = xt::cumsum(xt::view(mesh_dataset, xt::all(), 0)); // [120]
    // std::cout << "PASS " << timeInfo.size() << std::endl;
    xt::xarray<double> intrTime = xt::arange(timeInfo[0], timeInfo[timeInfo.size() - 1], (1.0f / m_fsRe)); // [397]
    // std::cout << "PASS" << std::endl;

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
    // std::cout << "PASS" << std::endl;

    int st_idx = -(m_targetTime * m_fsRe); // -400
    auto p_rgbInter = xt::view(rgbInter, xt::range(st_idx, xt::placeholders::_), xt::all()); // [397, 3]
    auto p_intrTime = xt::view(intrTime, xt::range(st_idx, xt::placeholders::_)); // [397]
    // std::cout << "PASS" << std::endl;

    int N = static_cast<int>(p_intrTime.shape()[0]); // [397]
    xt::xarray<double> rPPGmDat = xt::zeros<double>({N}); // [397] == H
    int l = (int)std::ceil(m_winSec * m_fsRe); // 80
    // auto C = xt::zeros<double>({l, 3}); // [80, 3]
    xt::xarray<double> P = {{0.0, 1.0, -1.0}, {-2.0, 1.0, 1.0}}; // [2, 3]
    // std::cout << "PASS" << std::endl;

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
    // std::cout << "PASS" << std::endl;

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

    Logi("fill size : ", fillList.size());
    xt::xarray<double> inputt = {0.5};
    for(int i = 0; i < lenList; i++) {
        if (i == 0) fillList = inputt;
        else fillList = xt::concatenate(xt::xtuple(fillList, inputt)); // append 0.5
    }

    xt::xarray<double> data_1_1;
    if (data11.size() >= 400) data_1_1 = data11; // lower than 15 fps
    else data_1_1 = xt::concatenate(xt::xtuple(data11, fillList), 0); // [400]

    xt::xarray<double> data_2_1;
    if (data21.size() >= 400) data_2_1 = data21;
    else data_2_1 = xt::concatenate(xt::xtuple(data21, fillList), 0); // [400]

    xt::xarray<double> inputDat = xt::stack(xt::xtuple(data_1_1, data_2_1), 1); // [400, 2]
    // std::cout << "Try this" << std::endl;
    // memoryDump(inputDat.data(), "./face_rgb_3m.bin", 400 * 2 * sizeof(double));
    // std::cout << "END" << std::endl;

    xt::xarray<double> reshapeData = inputDat.reshape({1, inputDat.shape()[0], 2 }); // [1, 400, 2]
    xt::xarray<float> double_reshapeData = xt::cast<float>(reshapeData);

    cv::Mat cv_reshapeData(double_reshapeData.shape()[0], double_reshapeData.shape()[1], CV_32FC2, double_reshapeData.data());
    // std::cout << "Try this" << std::endl;
    // memoryRestore(cv_reshapeData.data, "/usr/share/aif/example/face_rgb_scale_3m.bin");
    // std::cout << "END" << std::endl;
    descriptor->setImage(cv_reshapeData);

    return true;
}

xt::xarray<double> RppgPreProcessOperation::dotProduct(xt::xarray<double>& p, xt::xarray<double>& cn)
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

} // end of namespace aif
