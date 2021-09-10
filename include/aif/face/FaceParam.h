#ifndef AIF_FACE_PARAM_H
#define AIF_FACE_PARAM_H

#include <aif/base/DetectorParam.h>

#include <memory>
#include <vector>
#include <ostream>

namespace aif {

class FaceParam : public DetectorParam
{
public:
    FaceParam(
        const std::vector<int>& strides = {8, 16, 16, 16},
        const std::vector<float>& optAspectRatios = {1.0f},
        float interpolatedScaleAspectRatio = 1.0f,
        float anchorOffsetX = 0.5,
        float anchorOffsetY = 0.5,
        float minScale = 0.1484375,
        float maxScale = 0.75,
        bool  reduceBoxesInLowestLayer = false,
        float scoreThreshold = 0.7f,
        float iouThreshold = 0.2f,
        int   maxOutputSize = 100
    );

    FaceParam(const FaceParam& other);
    FaceParam(FaceParam&& other) noexcept;

    FaceParam& operator=(const FaceParam& other);
    FaceParam& operator=(FaceParam&& other) noexcept;

    bool operator==(const FaceParam& other) const;
    bool operator!=(const FaceParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const FaceParam& fp);
    void trace();

public:
    // for anchor
    std::vector<int> strides;
    std::vector<float> optAspectRatios;
    float interpolatedScaleAspectRatio;
    float anchorOffsetX;
    float anchorOffsetY;
    float minScale;
    float maxScale;
    bool  reduceBoxesInLowestLayer = false;
    // for detect
    float scoreThreshold;
    float iouThreshold;
    int   maxOutputSize;
};

} // end of namespace aif

#endif // AIF_FACE_PARAM_H
