#ifndef AIF_POSELANDMARK_PARAM_H
#define AIF_POSELANDMARK_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>

#include <memory>
#include <vector>
#include <ostream>

namespace aif {

class PoseLandmarkParam : public DetectorParam
{
public:
    PoseLandmarkParam();
    virtual ~PoseLandmarkParam();
    PoseLandmarkParam(const PoseLandmarkParam& other);
    PoseLandmarkParam(PoseLandmarkParam&& other) noexcept;

    PoseLandmarkParam& operator=(const PoseLandmarkParam& other);
    PoseLandmarkParam& operator=(PoseLandmarkParam&& other) noexcept;

    bool operator==(const PoseLandmarkParam& other) const;
    bool operator!=(const PoseLandmarkParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const PoseLandmarkParam& fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_POSELANDMARK_PARAM_H
