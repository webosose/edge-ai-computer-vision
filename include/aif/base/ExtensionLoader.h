/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <aif/base/Types.h>

#include <memory>

#ifndef EDGEAI_VISION_EXTENSION_PATH
#define EDGEAI_VISION_EXTENSION_PATH "/usr/lib/edgeai-extensions"
#endif

#ifndef EDGEAI_VISION_EXTENSION_REGISTRY_PATH
#define EDGEAI_VISION_EXTENSION_REGISTRY_PATH "/tmp/edgeai_extension_registry.json"
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

  t_aif_status initRetry(bool readRegistryFile=false, const std::string& extensionDirectoryPath = EDGEAI_VISION_EXTENSION_PATH,
    const std::vector<std::string>& allowedExtensionNames = {}, bool forceRetry = false) noexcept;

  t_aif_status init(bool readRegistryFile=false, const std::string& extensionDirectoryPath = EDGEAI_VISION_EXTENSION_PATH,
    const std::vector<std::string>& allowedExtensionNames = {}) noexcept;

  bool isInitialized() { return m_initDone; }
  t_aif_status isAvailable(const std::string& featureName, t_feature_type type, const std::string& pluginName="");
  t_aif_status enableFeature(const std::string& featureName, t_feature_type type, const std::string& pluginName="");
  t_aif_status loadExtension(const std::string& extensionFilePath);
  t_aif_status unloadAllExtension();
  t_aif_status clear();
  std::vector<aif::t_aif_plugin_info> getPluginInfos() { return m_pluginInfos; }
  std::vector<aif::t_aif_feature_info> getFeatureInfos() { return m_featureInfos; }
  std::string featureTypeToString(t_feature_type type);
  t_feature_type stringToFeatureType(const std::string& type);
  std::string getRegistryStampFilePath(bool create=false);
  std::string getRegistryFilePath() { return m_registryFilePath; }
  void setRetryCount(int retryCount) noexcept { m_retryCount = retryCount;}
  int getRetryCount() noexcept { return m_retryCount; }
  void setFaultTolerance(bool faultTolerance) noexcept { m_faultTolerance = faultTolerance; }
  bool getFaultTolerance() noexcept { return m_faultTolerance; }

private:
  ExtensionLoader(): m_registryFilePath(EDGEAI_VISION_EXTENSION_REGISTRY_PATH)
  {}
  ExtensionLoader(const ExtensionLoader&): m_registryFilePath(EDGEAI_VISION_EXTENSION_REGISTRY_PATH)
  {}
  ~ExtensionLoader();

  t_aif_status initFromRegistryFile();
  bool isFeatureNameHasPluginAlias(const std::string& feature_name);
  bool isAllowedExtension(const std::string& extensionName);
  bool isNeededToGenRegistryFile() noexcept;
  bool isReadableDirectory(const std::string& path);
  std::string getPluginNameByAlias(const std::string& alias);
  std::string getPluginAliasByName(const std::string& name);
  std::string getBaseFileName(const std::string& name) noexcept;
  bool compareExtensionName(const std::string& name1, const std::string& name2);
  t_aif_status runInspector();
  t_aif_parsed_feature_name parseFeatureName(const std::string& feature_name);

  t_aif_status setExtensionDirectory(const std::string& extensionDirectoryPath);
  t_aif_status setAllowedExtensionNames(const std::vector<std::string>& allowedExtensionNames);

private:
  bool m_initDone = false;
  bool m_allowAllExtensions = true;

  int  m_retryCount = 1;
  bool m_faultTolerance = false;

  std::string m_registryFilePath;
  std::string m_extensionDirectoryPath;
  std::vector<std::string> m_allowedExtensionNames;

  std::vector<aif::t_aif_plugin_info> m_pluginInfos;
  std::vector<aif::t_aif_feature_info> m_featureInfos;
};

} // namespace aif