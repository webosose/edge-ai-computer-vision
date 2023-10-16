/*
* Copyright (c) 2023 LG Electronics Inc.
* SPDX-License-Identifier: Apache-2.0
*/
// Code from AI Research Center

#include <aif/rppg/CubicSpline.h>
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xindex_view.hpp>


namespace aif {

CubicSpline::CubicSpline()
: m_a()
, m_b()
, m_c()
, m_d()
, m_x0()
{
}

CubicSpline::~CubicSpline()
{
}

void CubicSpline::CSpline(xt::xarray<double> x, xt::xarray<double> y)
{
    calcSplineParams(x, y);
    m_x0 = x;
}

xt::xarray<double> CubicSpline::CalcCSpline(xt::xarray<double> x)
{
    return pieceWiseSpline( x, m_x0, m_a, m_b, m_c, m_d);
}

xt::xarray<double> CubicSpline::triDiagSolve (xt::xarray<double> A, xt::xarray<double> B, xt::xarray<double> C, xt::xarray<double> F)
{
    xt::xarray<double> Bs, Fs, x;
    int n = B.size();

    Bs = xt::zeros<double>(B.shape());
    Fs = xt::zeros<double>(F.shape());

    Bs[0] = B[0];
    Fs[0] = F[0];

    for (int i = 1; i < n; ++i) {
        Bs[i] = B[i] - A[i] / Bs[i - 1] * C[i - 1];
        Fs[i] = F[i] - A[i] / Bs[i - 1] * Fs[i - 1];
    }

    x = xt::zeros<double>(B.shape());

    x.back() = Fs.back() / Bs.back();

    for (int i = n - 2; i >= 0; --i) {
        x[i] = (Fs[i] - C[i] * x[i + 1]) / Bs[i];
    }

    return x;
}

int CubicSpline::calcSplineParams( xt::xarray<double> x, xt::xarray<double> y)
{
    xt::xarray<double> h;
    xt::xarray<double> tri_diag;
    xt::xarray<double> A, B, C, F;
    xt::xarray<double> scalar = {0.0};
    xt::xarray<double> ret;

    m_a = y;
    h = xt::diff(x);
    m_c = xt::zeros<double>({1});

    A = xt::view(h, xt::range(xt::placeholders::_, -1));
    C = xt::view(h, xt::range(1, xt::placeholders::_));
    B = (A + C) * 2;
    F = ((xt::view(m_a, xt::range(2, xt::placeholders::_)) - xt::view(m_a, xt::range(1, -1))) / xt::view(h, xt::range(1, xt::placeholders::_))
        - ( xt::view(m_a, xt::range(1, -1)) - xt::view(m_a, xt::range(xt::placeholders::_, -2))) / xt::view(h, xt::range(xt::placeholders::_, -1))) * 3;

    tri_diag = triDiagSolve( A, B, C, F);
    m_c = xt::concatenate(xt::xtuple(m_c, tri_diag, scalar), 0);

    m_d = xt::diff(m_c) / (3 * h);

    m_b = (xt::view(m_a, xt::range(1, xt::placeholders::_)) - xt::view(m_a, xt::range(xt::placeholders::_, -1))) / h
        + (2 * xt::view(m_c, xt::range(1, xt::placeholders::_)) + xt::view(m_c, xt::range(xt::placeholders::_, -1))) / 3 * h;

    m_a = xt::view(m_a, xt::range(1, xt::placeholders::_));
    m_c = xt::view(m_c, xt::range(1, xt::placeholders::_));

    return 0;
}

xt::xarray<double> CubicSpline::funcSpline(xt::xarray<double> x, xt::xarray<std::int64_t> ix, xt::xarray<double> x0,
                                                xt::xarray<double> a, xt::xarray<double> b, xt::xarray<double> c,xt::xarray<double> d)
{
    xt::xarray<double> dx;
    xt::xarray<double> ret;

    dx = xt::view(x0, xt::range(1, xt::placeholders::_));
    dx = x - xt::index_view(dx, ix);
    ret = xt::index_view(a, ix) + (xt::index_view(b, ix) + ( xt::index_view(c, ix) + xt::index_view(d, ix) * dx) * dx) * dx;

    return ret;
}

xt::xarray<std::int64_t> CubicSpline::searchsortedMerge(xt::xarray<double> a, xt::xarray<double> b)
{
    std::size_t len_b = b.size();
    xt::xarray<std::int64_t> ix = xt::zeros<std::int64_t>({len_b});

    std::size_t pa = 0;
    std::size_t pb = 0;

    while (pb < len_b) {
        if (pa < a.size() && a(pa) < b(pb)) {
            pa += 1;
        } else {
            ix(pb) = pa;
            pb += 1;
        }
    }

    return ix;
}

xt::xarray<double> CubicSpline::pieceWiseSpline(xt::xarray<double> x, xt::xarray<double> x0, xt::xarray<double> a,
                            xt::xarray<double> b, xt::xarray<double> c, xt::xarray<double> d)
{
    xt::xarray<std::int64_t> ix;
    xt::xarray<double> y;
    xt::dynamic_shape<std::size_t> xsh = x.shape();

    x = xt::flatten(x);
    ix = searchsortedMerge(xt::view(x0, xt::range(1, -1)), x);
    y = funcSpline(x, ix, x0, a, b, c, d);
    y = y.reshape(xsh);

    return y;
}

} // end of namespace aif
