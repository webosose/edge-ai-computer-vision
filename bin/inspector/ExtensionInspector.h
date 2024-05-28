/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <aif/base/AIVision.h>
#include <aif/base/ExtensionLoader.h>
#include <aif/base/Types.h>
#include <aif/log/Logger.h>

#include <string>
#include <vector>

namespace aif {

class ExtensionInspector {
public:
  static ExtensionInspector& get()
  {
    static ExtensionInspector instance;
    return instance;
  }
  void setPluginPath(const std::string& pluginPath=EDGEAI_VISION_EXTENSION_PATH) { m_pluginPath = pluginPath; }
  t_aif_status inspect();
  std::string json();
  t_aif_status dump(std::string dumpPath);

private:
  ExtensionInspector(){};
  ~ExtensionInspector(){};
  void init();

private:
  std::string m_pluginPath;
  std::vector<aif::t_aif_plugin_info> m_pluginInfos;
  std::vector<aif::t_aif_feature_info> m_featureInfos;
};

} // namespace aif