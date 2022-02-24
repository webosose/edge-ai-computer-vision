#ifndef AIF_BODYPIX_PARAM_H
#define AIF_BODYPIX_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>

#include <memory>
#include <vector>
#include <ostream>

namespace aif {

class BodypixParam : public DetectorParam
{
public:
    BodypixParam();
    virtual ~BodypixParam();
    BodypixParam(const BodypixParam& other);
    BodypixParam(BodypixParam&& other) noexcept;

    BodypixParam& operator=(const BodypixParam& other);
    BodypixParam& operator=(BodypixParam&& other) noexcept;

    bool operator==(const BodypixParam& other) const;
    bool operator!=(const BodypixParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const BodypixParam& fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_BODYPIX_PARAM_H
