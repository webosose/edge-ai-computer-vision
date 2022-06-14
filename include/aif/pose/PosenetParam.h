#ifndef AIF_POSENET_PARAM_H
#define AIF_POSENET_PARAM_H

#include <aif/base/DetectorParam.h>
#include <aif/base/Types.h>

#include <memory>
#include <ostream>
#include <vector>

namespace aif {

class PosenetParam : public DetectorParam {
  public:
    PosenetParam();
    virtual ~PosenetParam();
    PosenetParam(const PosenetParam &other);
    PosenetParam(PosenetParam &&other) noexcept;

    bool operator==(const PosenetParam &other) const;
    bool operator!=(const PosenetParam &other) const;

    // debug
    friend std::ostream &operator<<(std::ostream &os, const PosenetParam &fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_POSENET_PARAM_H
