#ifndef AIF_SEMANTIC_PARAM_H
#define AIF_SEMANTIC_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>

#include <memory>
#include <vector>
#include <ostream>

namespace aif {

class SemanticParam : public DetectorParam
{
public:
    SemanticParam();
    virtual ~SemanticParam();
    SemanticParam(const SemanticParam& other);
    SemanticParam(SemanticParam&& other) noexcept;

    SemanticParam& operator=(const SemanticParam& other);
    SemanticParam& operator=(SemanticParam&& other) noexcept;

    bool operator==(const SemanticParam& other) const;
    bool operator!=(const SemanticParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const SemanticParam& fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_SEMANTIC_PARAM_H
