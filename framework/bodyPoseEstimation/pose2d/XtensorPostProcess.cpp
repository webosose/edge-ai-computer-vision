#include <aif/bodyPoseEstimation/pose2d/XtensorPostProcess.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <limits>
#include <cmath>
#if defined(USE_XTENSOR)
#include <xtensor/xsort.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xadapt.hpp>
#endif

namespace aif {

XtensorPostProcess::XtensorPostProcess(std::shared_ptr<Pose2dDetector>& detector)
    : PostProcess(detector)
{
}

XtensorPostProcess::~XtensorPostProcess()
{
}

bool XtensorPostProcess::execute(std::shared_ptr<Descriptor>& descriptor, float* data)
{
    Logd("Xtensor Post Process!!!");
#if defined(USE_XTENSOR)
    int totalVolume = m_numInputs * m_numKeyPoints * m_heatMapHeight * m_heatMapWidth;
    std::vector<int> shape = {m_numInputs, m_numKeyPoints, m_heatMapHeight*m_heatMapWidth};
    xt::xarray<float> batchHeatmaps = xt::adapt(data, totalVolume, xt::no_ownership(), shape);

    if(!processHeatMap(descriptor, batchHeatmaps)) return false;
#endif
    return true;
}

#if defined(USE_XTENSOR)
bool XtensorPostProcess::processHeatMap(std::shared_ptr<Descriptor>& descriptor, xt::xarray<float>& batchHeatmaps)
{
    std::vector<std::vector<float>> keyPoints;

    xt::xarray<int> argMaxIndex = xt::argmax(batchHeatmaps, 2); // getFinalPredsNotransform + getMaxPreds
    xt::xarray<float> maxVal = xt::amax(batchHeatmaps, 2);

    xt::xarray<int> x = (argMaxIndex % m_heatMapWidth);//preds_width
    xt::xarray<int> y = (argMaxIndex / m_heatMapWidth); //preds_height

    xt::xarray<float> coords = xt::stack(xt::xtuple(x, y), 2);

#if defined(GAUSSIANDARK)
    Logd("GaussianDark!!!");
    gaussianDark(batchHeatmaps, coords); // getFinalPredsNotransform
#endif
    for (auto i = 0; i < m_numInputs; i++) { // batch = 1
        for (auto j = 0; j < m_numKeyPoints; j++) { // 41
            if (m_useUDP) {
                keyPoints.push_back({maxVal(i, j), coords(i, j, 0), coords(i, j, 1), 1.0});
            } else {
                int img_idx = i * m_numKeyPoints * 2;
                float mul[3][3];
                getTransformMatrix(m_cropBbox, mul);
                float x = (coords[img_idx + j * 2] * mul[0][0]) + mul[2][0];
                float y = (coords[img_idx + j * 2 + 1] * mul[1][1]) + mul[2][1];

                keyPoints.push_back({maxVal(i, j), x, y});
            }
        }
    }

    if (m_useUDP) {
        if (!applyInverseTransform(keyPoints)) {
            return false;
        }
    }

    std::shared_ptr<Pose2dDescriptor> pose2dDescriptor =
        std::dynamic_pointer_cast<Pose2dDescriptor>(descriptor);
    if (pose2dDescriptor == nullptr) {
        Loge(__func__, "failed to convert Descriptor to Pose2dDescriptor");
        return false;
    }
    pose2dDescriptor->addKeyPoints(keyPoints);
    return true;
}

void XtensorPostProcess::gaussianBlur(xt::xarray<float>& batchHeatmaps, const int BLUR_KERNEL) const
{
    for (auto i = 0; i < m_numInputs; i++) {
        for (auto j = 0; j < m_numKeyPoints; j++) {
            xt::xarray<float> keyPointHeatmap = xt::view(batchHeatmaps, i, j);
            cv::Mat heatmap(m_heatMapHeight, m_heatMapWidth, CV_32F, keyPointHeatmap.begin());

            double originMin, originMax;
            cv::minMaxLoc (heatmap, &originMin, &originMax); // find global minimum and maximum values and posigion

            cv::Mat heatMapBlur_mat;
            cv::GaussianBlur(heatmap, heatMapBlur_mat, cv::Size(BLUR_KERNEL, BLUR_KERNEL), 0., 0., cv::BORDER_CONSTANT);

            double newMin, newMax;
            cv::minMaxLoc(heatMapBlur_mat, &newMin, &newMax);
            std::copy(heatMapBlur_mat.begin<float>(), heatMapBlur_mat.end<float>(),
                    keyPointHeatmap.begin()); // copy heatMapBlur_map and paste at keyPointHeatmap's begin

            keyPointHeatmap = keyPointHeatmap * (originMax / newMax);
            xt::view(batchHeatmaps, i, j, xt::all()) = keyPointHeatmap;
        }
    }
}

void XtensorPostProcess::taylor(const xt::xarray<float>& heatMap , xt::xarray<float>& coord) const
{
    int px = static_cast<int>(coord(0));
    int py = static_cast<int>(coord(1));

    if ((1 < px) && (px < (m_heatMapWidth - 2)) && (1 < py) && (py < (m_heatMapHeight - 2))) {
        auto curLine = py * m_heatMapWidth;
        auto nextLine = ( py + 1 ) * m_heatMapWidth;
        auto prevLine = ( py - 1 ) * m_heatMapWidth;
        auto nnLine = ( py + 2 ) * m_heatMapWidth;
        auto ppLine = ( py - 2 ) * m_heatMapWidth;
        auto dx = 0.5f * (heatMap(curLine + px + 1) - heatMap(curLine + px - 1));
        auto dy = 0.5f * (heatMap(nextLine + px) - heatMap(prevLine + px));
        auto dxx = 0.25f * (heatMap(curLine + px + 2) - (2.f * heatMap(curLine + px)) + heatMap(curLine + px - 2));
        auto dxy = 0.25f *(heatMap(nextLine + px + 1) - heatMap(prevLine + px + 1)
                - heatMap(nextLine + px - 1) + heatMap(prevLine + px - 1));
        auto dyy = 0.25f * (heatMap(nnLine + px) - (2.f * heatMap(curLine + px)) + heatMap(ppLine + px));

        float det = (dxx * dyy) - (dxy * dxy);

        if (!floatEquals(det, 0.f)) {
            xt::xarray<float> hessianAdjointInv = {{dyy/det, -dxy/det}, {-dxy/det, dxx/det}};
            float offsetx = -((hessianAdjointInv(0, 0) * dx) + (hessianAdjointInv(0, 1) * dy));
            float offsety = -((hessianAdjointInv(1, 0) * dx) + (hessianAdjointInv(1, 1) * dy));

            coord(0) = coord(0) + offsetx;
            coord(1) = coord(1) + offsety;
        }
    }
}

void XtensorPostProcess::gaussianDark(xt::xarray<float>& batchHeatmaps, xt::xarray<float>& coords) const
{
    gaussianBlur(batchHeatmaps);
    batchHeatmaps = xt::maximum(batchHeatmaps, 1e-10); // elemenwise-maximum. same as np.maximum()
    batchHeatmaps = xt::log(batchHeatmaps);

    for (auto i = 0; i < m_numInputs; i++) {
        for (auto j = 0; j < m_numKeyPoints; j++) {
            xt::xarray<float> heatMap = xt::view(batchHeatmaps, i, j);
            xt::xarray<float> coord = xt::view(coords, i, j);

            taylor(heatMap, coord);

            coords(i, j, 0) = coord(0);
            coords(i, j, 1) = coord(1);
        }
    }
}
#endif

} // end of namespace aif
