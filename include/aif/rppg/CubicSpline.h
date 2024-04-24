/*
* Copyright (c) 2023 LG Electronics Inc.
* SPDX-License-Identifier: Apache-2.0
*/
// Code from AI Research Center

#ifndef AIF_CUBIC_SPLINE_H
#define AIF_CUBIC_SPLINE_H

#include <xtensor/xarray.hpp>

namespace aif {

class CubicSpline
{
public:
    CubicSpline();
    virtual ~CubicSpline();

    void CSpline(xt::xarray<double> x, xt::xarray<double> y);
    xt::xarray<double> CalcCSpline(xt::xarray<double> x);
private:
    xt::xarray<double> triDiagSolve (xt::xarray<double> A, xt::xarray<double> B, xt::xarray<double> C, xt::xarray<double> F);
    xt::xarray<double> pieceWiseSpline (xt::xarray<double> x, xt::xarray<double> x0, xt::xarray<double> a,
                                xt::xarray<double> b, xt::xarray<double> c, xt::xarray<double> d);
    int calcSplineParams(xt::xarray<double> x, xt::xarray<double> y);
    xt::xarray<std::int64_t> searchsortedMerge(xt::xarray<double> a, xt::xarray<double> b);
    xt::xarray<double> funcSpline(xt::xarray<double> x, xt::xarray<std::int64_t> ix, xt::xarray<double> x0, xt::xarray<double> a,
                                    xt::xarray<double> b, xt::xarray<double> c, xt::xarray<double> d);
private:
    xt::xarray<double> m_a, m_b, m_c, m_d, m_x0;
};

} // end of namespace aif

#endif // AIF_CUBIC_SPLINE_H