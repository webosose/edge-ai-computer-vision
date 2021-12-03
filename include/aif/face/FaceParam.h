#ifndef AIF_FACE_PARAM_H
#define AIF_FACE_PARAM_H

#include <aif/base/DetectorParam.h>

#include <memory>
#include <vector>
#include <string>
#include <ostream>

namespace aif {

class FaceParam : public DetectorParam
{
public:
    FaceParam();
    FaceParam(const FaceParam& other);
    FaceParam(FaceParam&& other) noexcept;

    FaceParam& operator=(const FaceParam& other);
    FaceParam& operator=(FaceParam&& other) noexcept;

    bool operator==(const FaceParam& other) const;
    bool operator!=(const FaceParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const FaceParam& fp);
    void trace();


    t_aif_status fromJson(const std::string& param) override;

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
    float updateThreshold;
};

} // end of namespace aif

#endif // AIF_FACE_PARAM_H
