#include <aif/face/FaceParam.h>
#include <aif/base/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

FaceParam::FaceParam(
        const std::vector<int>& _strides,
        const std::vector<float>& _optAspectRatios,
        float _interpolatedScaleAspectRatio,
        float _anchorOffsetX,
        float _anchorOffsetY,
        float _minScale,
        float _maxScale,
        bool  _reduceBoxesInLowestLayer,
        float _scoreThreshold,
        float _iouThreshold,
        int   _maxOutputSize
    )
    : DetectorParam()
    , strides{_strides}
    , optAspectRatios{_optAspectRatios}
    , interpolatedScaleAspectRatio{_interpolatedScaleAspectRatio}
    , anchorOffsetX{_anchorOffsetX}
    , anchorOffsetY{_anchorOffsetY}
    , minScale{_minScale}
    , maxScale{_maxScale}
    , reduceBoxesInLowestLayer{_reduceBoxesInLowestLayer}
    , scoreThreshold{_scoreThreshold}
    , iouThreshold{_iouThreshold}
    , maxOutputSize{_maxOutputSize}
{
}

FaceParam::FaceParam(const FaceParam& other)
    : strides{other.strides}
    , optAspectRatios{other.optAspectRatios}
    , interpolatedScaleAspectRatio{other.interpolatedScaleAspectRatio}
    , anchorOffsetX{other.anchorOffsetX}
    , anchorOffsetY{other.anchorOffsetY}
    , minScale{other.minScale}
    , maxScale{other.maxScale}
    , reduceBoxesInLowestLayer{other.reduceBoxesInLowestLayer}
    , scoreThreshold{other.scoreThreshold}
    , iouThreshold{other.iouThreshold}
    , maxOutputSize{other.maxOutputSize}
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

FaceParam::FaceParam(FaceParam&& other) noexcept
    : strides{std::move(other.strides)}
    , optAspectRatios{std::move(other.optAspectRatios)}
    , interpolatedScaleAspectRatio{std::exchange(other.interpolatedScaleAspectRatio, 1.0f)}
    , anchorOffsetX{std::exchange(other.anchorOffsetX, 0.5)}
    , anchorOffsetY{std::exchange(other.anchorOffsetY, 0.5)}
    , minScale{std::exchange(other.minScale, 0.1484375)}
    , maxScale{std::exchange(other.maxScale, 0.75)}
    , reduceBoxesInLowestLayer{std::exchange(other.reduceBoxesInLowestLayer, false)}
    , scoreThreshold{std::exchange(other.scoreThreshold, 0.7f)}
    , iouThreshold{std::exchange(other.iouThreshold, 0.2f)}
    , maxOutputSize{std::exchange(other.maxOutputSize, 100)}
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

FaceParam& FaceParam::operator=(const FaceParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    strides = other.strides;
    optAspectRatios = other.optAspectRatios;
    interpolatedScaleAspectRatio = other.interpolatedScaleAspectRatio;
    anchorOffsetX = other.anchorOffsetX;
    anchorOffsetY = other.anchorOffsetY;
    minScale = other.minScale;
    maxScale = other.maxScale;
    reduceBoxesInLowestLayer = other.reduceBoxesInLowestLayer;
    scoreThreshold = other.scoreThreshold;
    iouThreshold = other.iouThreshold;
    maxOutputSize = other.maxOutputSize;

    return *this;
}

FaceParam& FaceParam::operator=(FaceParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    strides = std::move(other.strides);
    optAspectRatios = std::move(other.optAspectRatios);
    interpolatedScaleAspectRatio = std::exchange(other.interpolatedScaleAspectRatio, 1.0f);
    anchorOffsetX = std::exchange(other.anchorOffsetX, 0.5);
    anchorOffsetY = std::exchange(other.anchorOffsetY, 0.5);
    minScale = std::exchange(other.minScale, 0.1484375);
    maxScale = std::exchange(other.maxScale, 0.75);
    reduceBoxesInLowestLayer = std::exchange(other.reduceBoxesInLowestLayer, false);
    scoreThreshold = std::exchange(other.scoreThreshold, 0.7f);
    iouThreshold = std::exchange(other.iouThreshold, 0.2f);
    maxOutputSize = std::exchange(other.maxOutputSize, 100);

    return *this;
}

bool FaceParam::operator==(const FaceParam& other) const
{
    return (
        (std::equal(strides.begin(), strides.end(), other.strides.begin())) &&
        (std::equal(optAspectRatios.begin(), optAspectRatios.end(), other.optAspectRatios.begin())) &&
        (std::abs(interpolatedScaleAspectRatio - other.interpolatedScaleAspectRatio) < aif::EPSILON) &&
        (std::abs(anchorOffsetX - other.anchorOffsetX) < aif::EPSILON) &&
        (std::abs(anchorOffsetY - other.anchorOffsetY) < aif::EPSILON) &&
        (std::abs(minScale - other.minScale) < aif::EPSILON) &&
        (std::abs(maxScale - other.maxScale) < aif::EPSILON) &&
        (reduceBoxesInLowestLayer == other.reduceBoxesInLowestLayer) &&
        (std::abs(scoreThreshold - other.scoreThreshold) < aif::EPSILON) &&
        (std::abs(iouThreshold - other.iouThreshold) < aif::EPSILON) &&
        (maxOutputSize == other.maxOutputSize)
    );
}

bool FaceParam::operator!=(const FaceParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const FaceParam& fp)
{
    os << "\n{\n";
    os << "\tstrides: [";
    for (int i = 0; i < fp.strides.size(); i++) {
        if (i == 0) {
            os << fp.strides[i];
        } else {
            os << ", " << fp.strides[i];
        }
    }
    os << "],\n";
    os << "\toptAspectRatio: [";
    for (int i = 0; i < fp.optAspectRatios.size(); i++) {
        if (i == 0) {
            os << fp.optAspectRatios[i];
        } else {
            os << ", " << fp.optAspectRatios[i];
        }
    }
    os << "],\n";
    os << "\tinterpolatedScaleAspectRatio: " << fp.interpolatedScaleAspectRatio << ",\n";
    os << "\tanchorOffsetX: " << fp.anchorOffsetX << ",\n";
    os << "\tanchorOffsetY: " << fp.anchorOffsetY << ",\n";
    os << "\tminScale: " << fp.minScale << ",\n";
    os << "\tmaxScale: " << fp.maxScale << ",\n";
    os << "\treduceBoxesInLowestLayer: " << (fp.reduceBoxesInLowestLayer ? "true" : "false") << ",\n";
    os << "\tscoreThreshold: " << fp.scoreThreshold << ",\n";
    os << "\tiouThreshold: " << fp.iouThreshold << ",\n";
    os << "\tmaxOutputSize: " << fp.maxOutputSize << "\n";
    os << "}";

    return os;
}

void FaceParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

} // end of namespace aif
