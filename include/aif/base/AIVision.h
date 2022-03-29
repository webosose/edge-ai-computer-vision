#ifndef AIF_AIVISION_H
#define AIF_AIVISION_H

#include <aif/tools/ConfigReader.h>

#include <memory>

#ifndef EDGEAI_VISION_HOME
#define EDGEAI_VISION_HOME "/usr/share/aif"
#endif

#ifndef EDGEAI_VISION_CONFIG
#define EDGEAI_VISION_CONFIG "aivision.config"
#endif

namespace aif {

class AIVision {
public:
    static void init();
    static void deinit();
    static bool isInitialized();
    static std::string getBasePath();
    static std::string getModelFolderPath();

private:
    static bool s_initialized;
    static std::unique_ptr<ConfigReader> s_config;
};

} // end of namespace aif

#endif
