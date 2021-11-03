#ifndef AIF_SELFIE_PARAM_H
#define AIF_SELFIE_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>

#include <memory>
#include <vector>
#include <ostream>

namespace aif {

class SelfieParam : public DetectorParam
{
public:
    SelfieParam();
    virtual ~SelfieParam();
    SelfieParam(const SelfieParam& other);
    SelfieParam(SelfieParam&& other) noexcept;

    SelfieParam& operator=(const SelfieParam& other);
    SelfieParam& operator=(SelfieParam&& other) noexcept;

    bool operator==(const SelfieParam& other) const;
    bool operator!=(const SelfieParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const SelfieParam& fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_SELFIE_PARAM_H
