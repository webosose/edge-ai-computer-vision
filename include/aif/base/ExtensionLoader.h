#pragma once
#include <aif/base/Types.h>

#include <memory>

#ifndef EDGEAI_VISION_PLUGIN_PATH
#define EDGEAI_VISION_PLUGIN_PATH "/usr/lib/edgeai-extensions"
#endif

#ifndef EDGEAI_VISION_PLUGIN_INFO_DUMP_PATH
#define EDGEAI_VISION_PLUGIN_INFO_DUMP_PATH "/tmp/edgeai-extensions.json"
#endif

#ifndef EDGEAI_VISION_INSPECTOR_PATH
#define EDGEAI_VISION_INSPECTOR_PATH std::string(EDGEAI_VISION_HOME) + "/bin/edgeai-inspector"
#endif

namespace aif {

class ExtensionLoader
{
public:
  static ExtensionLoader& get()
  {
    static ExtensionLoader instance;
    return instance;
  }

  t_aif_status init(bool readFromDumpFile=false, std::string pluginPath = EDGEAI_VISION_PLUGIN_PATH);
  bool isInitialized() { return initDone; }
  t_aif_status isAvailable(std::string feature_name, t_feature_type type);
  t_aif_status enableFeature(std::string feature_name, t_feature_type type);
  t_aif_status loadExtension(std::string extensionFilePath);
  t_aif_status unloadAllExtension();
  t_aif_status clear();
  std::vector<aif::t_aif_plugin_info> getPluginInfos() { return plugin_infos; }
  std::vector<aif::t_aif_feature_info> getFeatureInfos() { return feature_infos; }
  std::string featureTypeToString(t_feature_type type);
  t_feature_type stringToFeatureType(std::string type);

private:
  ExtensionLoader() {}
  ExtensionLoader(const ExtensionLoader&) {}
  ~ExtensionLoader();

  t_aif_status initFromFile(std::string extensionInfoFilePath);

private:
  bool initDone = false;
  std::vector<aif::t_aif_plugin_info> plugin_infos;
  std::vector<aif::t_aif_feature_info> feature_infos;
};

} // namespace aif