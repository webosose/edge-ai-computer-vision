#include <aif/base/AIVision.h>
#include <aif/base/ExtensionLoader.h>
#include <aif/base/Types.h>
#include <aif/log/Logger.h>
#include <aif/base/IRegistration.h>

#include <dlfcn.h>
#include <iterator>
#include <string>
#include <vector>

using namespace aif;

#include <sstream>
#include <string>
#include <vector>
#include <map>

static void wait(std::string msg) {
  std::cout << msg << std::endl;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cin.get();
}

static std::vector<std::string> split(std::string str, char delimiter) {
  std::vector<std::string> splitted;
  std::istringstream iss(str);
  std::string token;
  while (std::getline(iss, token, delimiter)) {
    splitted.push_back(token);
  }
  return splitted;
}

static uint StrToDlFlag(const std::string& dlFlag) {
  std::string dlFlagUpper = dlFlag;
  std::transform(dlFlagUpper.begin(), dlFlagUpper.end(), dlFlagUpper.begin(), ::toupper);
  if (dlFlagUpper == "RTLD_LAZY") {
    return RTLD_LAZY;
  } else if (dlFlagUpper == "RTLD_NOW") {
    return RTLD_NOW;
  } else if (dlFlagUpper == "RTLD_GLOBAL") {
    return RTLD_GLOBAL;
  } else if (dlFlagUpper == "RTLD_LOCAL") {
    return RTLD_LOCAL;
  } else if (dlFlagUpper == "RTLD_DEEPBIND") {
    return RTLD_DEEPBIND;
  } else {
    return RTLD_LAZY;
  }
}

static std::string DlFlagToStr(uint dlFlag) {
  switch (dlFlag) {
    case RTLD_LAZY:
      return "RTLD_LAZY";
    case RTLD_NOW:
      return "RTLD_NOW";
    case RTLD_GLOBAL:
      return "RTLD_GLOBAL";
    case RTLD_LOCAL:
      return "RTLD_LOCAL";
    case RTLD_DEEPBIND:
      return "RTLD_DEEPBIND";
    default:
      return "RTLD_LAZY";
  }
}

// This sample demonstrates if the plugin can be loaded dynamically.
int main(int argc, char** argv) {
  std::map<uint, std::pair<std::string, std::string>> pluginPaths;

  //1. Print all available plugins
  Logger::init(Logger::strToLogLevel("DEBUG"));
  ExtensionLoader::get().init(true);
  auto pluginInfos = ExtensionLoader::get().getPluginInfos();
  uint idx = 1;
  for (auto& pluginInfo : pluginInfos) {
    Logi("Plugin name: ", pluginInfo.name);
    Logi("Plugin alias: ", pluginInfo.alias);
    Logi("Plugin path: ", pluginInfo.path);
    Logi("Plugin version: ", pluginInfo.version);
    Logi("Plugin description: ", pluginInfo.description);
    pluginPaths[idx++] = std::make_pair(pluginInfo.name, pluginInfo.path);
  }

  //2. Load / Unload plugin dynamically
  std::string dlFlags;
  uint dlFlag;
  void* handle;
  std::string tmp;
  while(true) {
    dlFlag = 0;
    // 2.1 Get user input for plugin idx
    for (auto it = pluginPaths.begin(); it != pluginPaths.end(); ++it) {
      std::cout << "idx:" << it->first << ", name:" << it->second.first << std::endl;
    }
    std::cout << "Enter plugin idx: ";
    std::cin >> idx;
    if (idx < 1 || idx > pluginPaths.size()) {
      Loge("Invalid plugin idx: ", idx);
      continue;
    }

    // 2.2 Get user input for dlopen flags
    std::cout << "Enter dlopen flags (RTLD_LAZY|RTLD_LOCAL|RTLD_NOW|RTLD_GLOBAL|RTLD_DEEPBIND): "; // RTLD_NOW|RTLD_GLOBAL|RTLD_DEEPBIND
    std::cin >> dlFlags;
    std::vector<std::string> dlFlagArray = split(dlFlags, '|');
    for (auto& dlFlagStr : dlFlagArray) {
      dlFlag |= StrToDlFlag(dlFlagStr);
    }

    // 2.3 Load plugin (AIF_EXTENSION_NAME is used in IRegistration constructor to get the extension name)
    //      Similar to the ExtensionLoader::get().loadExtension(pluginPaths[idx].second);
    setenv("AIF_EXTENSION_NAME", pluginPaths[idx].first.c_str(), 1);
    handle = dlopen(pluginPaths[idx].second.c_str(), dlFlag);
    if (!handle) {
      Loge("dlopen failed: ", dlerror());
      unsetenv("AIF_EXTENSION_NAME");
      continue;
    }
    unsetenv("AIF_EXTENSION_NAME");
    Logi("Plugin loaded: ", pluginPaths[idx].first, " with dlopen flags: ", dlFlags);

    // 2.4 Wait for user input to unload plugin
    wait("Press any key to unload the plugin.");

    // 2.5 Remove all registrations of the plugin (Just for testing purpose)
    auto regs = RegistrationJob::get().getRegistrations(pluginPaths[idx].first);
    Logi("Number of Registrations: ", regs.size());
    for (auto& reg : regs) {
      Logd("Delete Registration for ", reg.first);
      RegistrationJob::get().removeRegistration(pluginPaths[idx].first, reg.first);
    }

    // 2.6 Unload plugin
    //   Similar to the ExtensionLoader::get().unloadAllExtension();
    //   This sample does not guarantee the plugin is unloaded from the process memory although dlclose returns 0.
    if (dlclose(handle) != 0) {
      Loge("dlclose failed: ", dlerror());
      continue;
    }
    Logi("Plugin unloaded: ", pluginPaths[idx].first);
  }

  return 0;
}
