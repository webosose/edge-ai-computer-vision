#ifndef AIF_SEMANTIC_PARAM_H
#define AIF_SEMANTIC_PARAM_H

#include <aif/base/DetectorParam.h>
#include <aif/base/Types.h>

#include <memory>
#include <ostream>
#include <vector>

namespace aif {

class SemanticParam : public DetectorParam {
  public:
    SemanticParam();
    virtual ~SemanticParam();
    SemanticParam(const SemanticParam &other);
    SemanticParam(SemanticParam &&other) noexcept;

    bool operator==(const SemanticParam &other) const;
    bool operator!=(const SemanticParam &other) const;

    // debug
    friend std::ostream &operator<<(std::ostream &os, const SemanticParam &fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_SEMANTIC_PARAM_H
