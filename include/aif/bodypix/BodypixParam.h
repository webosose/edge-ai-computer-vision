#ifndef AIF_BODYPIX_PARAM_H
#define AIF_BODYPIX_PARAM_H

#include <aif/base/DetectorParam.h>
#include <aif/base/Types.h>

#include <memory>
#include <ostream>
#include <vector>

namespace aif {

class BodypixParam : public DetectorParam {
  public:
    BodypixParam();
    virtual ~BodypixParam();
    BodypixParam(const BodypixParam &other);
    BodypixParam(BodypixParam &&other) noexcept;

    bool operator==(const BodypixParam &other) const;
    bool operator!=(const BodypixParam &other) const;

    // debug
    friend std::ostream &operator<<(std::ostream &os, const BodypixParam &fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_BODYPIX_PARAM_H
