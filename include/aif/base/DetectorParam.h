/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_DETECTOR_PARAM_H
#define AIF_DETECTOR_PARAM_H

#include <aif/base/Types.h>
#include <string>
#include <vector>

namespace aif {

class DetectorParam {
  public:
    DetectorParam();
    virtual ~DetectorParam();

    enum { DEFAULT_NUM_THREADS = 1 };

    virtual t_aif_status fromJson(const std::string &param);
    virtual t_aif_status updateParam(const std::string &param);

    bool getUseXnnpack() const;
    int getNumThreads() const;
    const std::vector<std::pair<std::string, std::string>> &
    getDelegates() const;

    bool getUseAutoDelegate() const;
    std::string getAutoDelegateConfig() const;

  protected:
    std::string m_param;
    bool m_useAutoDelegate;
    bool m_useXnnpack;
    int m_numThreads;
    std::vector<std::pair<std::string, std::string>> m_delegates;
    std::string m_autoDelegateConfig;
};

} // end of namespace aif

#endif // AIF_DETECTOR_PARAM_H
