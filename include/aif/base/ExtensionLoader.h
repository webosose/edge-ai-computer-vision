#pragma once
#include <aif/base/Types.h>

#include <memory>

#ifndef EDGEAI_VISION_PLUGIN_PATH
#define EDGEAI_VISION_PLUGIN_PATH "/usr/lib/edgeai-extensions"
#endif

#ifndef EXTENSION_REGISTRY_PATH
#define EXTENSION_REGISTRY_PATH "/mnt/lg/cmn_data/edgeai/edgeai_plugin_registry.json"
#endif

#ifndef EDGEAI_VISION_INSPECTOR_PATH
#define EDGEAI_VISION_INSPECTOR_PATH std::string(EDGEAI_VISION_HOME) + "/bin/edgeai-inspector"
#endif

#ifndef NYX_CMD_PATH
#define NYX_CMD_PATH "/usr/bin/nyx-cmd"
#endif

#define FEATURE_NAME_DELIMITER "::"

namespace aif {

struct t_aif_parsed_feature_name
{
  std::string pluginAlias;
  std::string featureName;
};

class ExtensionLoader
{
public:
  static ExtensionLoader& get()
  {
    static ExtensionLoader instance;
    return instance;
  }

  t_aif_status init(bool readRegistryFile=false, std::string pluginPath = EDGEAI_VISION_PLUGIN_PATH, std::vector<std::string> allowedExtensionNames = {});
  bool isInitialized() { return m_initDone; }
  t_aif_status isAvailable(std::string featureName, t_feature_type type, std::string pluginName="");
  t_aif_status enableFeature(std::string featureName, t_feature_type type, std::string pluginName="");
  t_aif_status loadExtension(std::string extensionFilePath);
  t_aif_status unloadAllExtension();
  t_aif_status clear();
  std::vector<aif::t_aif_plugin_info> getPluginInfos() { return m_pluginInfos; }
  std::vector<aif::t_aif_feature_info> getFeatureInfos() { return m_featureInfos; }
  std::string featureTypeToString(t_feature_type type);
  t_feature_type stringToFeatureType(std::string type);
  std::string getRegistryStampFilePath(bool create=false);

private:
  ExtensionLoader(): m_registryFilePath(EXTENSION_REGISTRY_PATH)
  {}
  ExtensionLoader(const ExtensionLoader&): m_registryFilePath(EXTENSION_REGISTRY_PATH)
  {}
  ~ExtensionLoader();

  t_aif_status initFromRegistryFile();
  bool isFeatureNameHasPluginAlias(std::string feature_name);
  bool isAllowedExtension(std::string extensionName);
  bool isNeededToGenRegistryFile();
  std::string getPluginNameByAlias(std::string alias);
  std::string getPluginAliasByName(std::string name);
  t_aif_status runInspector(std::string pluginPath);
  t_aif_parsed_feature_name parseFeatureName(std::string feature_name);
  t_aif_status setAllowedExtensionNames(std::vector<std::string> allowedExtensionNames);

private:
  bool m_initDone = false;
  bool m_allowAllExtensions = true;
  std::string m_registryFilePath;
  std::vector<aif::t_aif_plugin_info> m_pluginInfos;
  std::vector<aif::t_aif_feature_info> m_featureInfos;
  std::vector<std::string> m_allowedExtensionNames;
};

} // namespace aif