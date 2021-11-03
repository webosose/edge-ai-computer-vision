#ifndef AIF_MOVENET_PARAM_H
#define AIF_MOVENET_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>

#include <memory>
#include <vector>
#include <ostream>

namespace aif {

class MovenetParam : public DetectorParam
{
public:
    MovenetParam();
    virtual ~MovenetParam();
    MovenetParam(const MovenetParam& other);
    MovenetParam(MovenetParam&& other) noexcept;

    MovenetParam& operator=(const MovenetParam& other);
    MovenetParam& operator=(MovenetParam&& other) noexcept;

    bool operator==(const MovenetParam& other) const;
    bool operator!=(const MovenetParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const MovenetParam& fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_MOVENET_PARAM_H
