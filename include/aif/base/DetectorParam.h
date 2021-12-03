#ifndef AIF_DETECTOR_PARAM_H
#define AIF_DETECTOR_PARAM_H

#include <aif/base/Types.h>
#include <string>
#include <vector>

namespace aif {

class DetectorParam
{
public:
    DetectorParam();
    virtual ~DetectorParam();

    enum { DEFAULT_NUM_THREADS = 1 };

    virtual t_aif_status fromJson(const std::string& param);

    bool getUseXnnpack() const;
    size_t getNumThreads() const;
    const std::vector<std::pair<std::string, std::string>>& getDelegates() const;

protected:
    std::string  m_param;
    bool m_useXnnpack;
    size_t m_numThreads;
    std::vector<std::pair<std::string, std::string>> m_delegates;
};

} // end of namespace aif

#endif // AIF_DETECTOR_PARAM_H
