/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ExtensionInspector.h"

#include <boost/filesystem.hpp>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <fstream>

namespace aif
{

void ExtensionInspector::init()
{
  std::unique_ptr<ConfigReader> config = std::make_unique<ConfigReader>(
            std::string(EDGEAI_VISION_HOME) + "/" + std::string(EDGEAI_VISION_CONFIG));
  Logger::init(Logger::strToLogLevel(config->getOption(KEY_LOG_LEVEL)));
}

t_aif_status ExtensionInspector::inspect()
{
  init();
  if (ExtensionLoader::get().init(false, m_pluginPath) != kAifOk)
  {
    Loge("Failed to initialize ExtensionLoader");
    return kAifError;
  }
  m_pluginInfos = ExtensionLoader::get().getPluginInfos();
  if (m_pluginInfos.empty())
  {
    Loge("No plugins found");
    return kAifError;
  }
  m_featureInfos = ExtensionLoader::get().getFeatureInfos();
  if (m_featureInfos.empty())
  {
    Loge("No features found");
    return kAifError;
  }
  std::sort(m_pluginInfos.begin(), m_pluginInfos.end(), [](const t_aif_plugin_info &a, const t_aif_plugin_info &b)
            { return a.name < b.name; });
  std::sort(m_featureInfos.begin(), m_featureInfos.end(), [](const t_aif_feature_info &a, const t_aif_feature_info &b)
            {
    if (a.pluginName == b.pluginName) {
      return a.type < b.type;
    }
    return a.pluginName < b.pluginName; });

  return kAifOk;
}

std::string ExtensionInspector::json()
{
  if (m_pluginInfos.empty() && m_featureInfos.empty())
  {
    inspect();
  }

  rapidjson::StringBuffer s;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
  writer.SetIndent(' ', 4); // Set the indent to 4 spaces
  writer.StartArray();
  for (auto &pluginInfo : m_pluginInfos)
  {
    writer.StartObject();
    writer.Key("name");
    writer.String(pluginInfo.name.c_str());
    writer.Key("path");
    writer.String(pluginInfo.path.c_str());
    if (pluginInfo.alias != "")
    {
      writer.Key("alias");
      writer.String(pluginInfo.alias.c_str());
    }
    if (pluginInfo.version != "")
    {
      writer.Key("version");
      writer.String(pluginInfo.version.c_str());
    }
    if (pluginInfo.description != "")
    {
      writer.Key("description");
      writer.String(pluginInfo.description.c_str());
    }
    writer.Key("features");
    writer.StartArray();
    for (auto &featureInfo : m_featureInfos)
    {
      if (featureInfo.pluginName == pluginInfo.name)
      {
        writer.StartObject();
        writer.Key("name");
        writer.String(featureInfo.name.c_str());
        writer.Key("type");
        writer.String(ExtensionLoader::get().featureTypeToString(featureInfo.type).c_str());
        writer.EndObject();
      }
    }
    writer.EndArray();
    writer.EndObject();
  }
  writer.EndArray();

  return s.GetString();
}

t_aif_status ExtensionInspector::dump(std::string dumpPath)
{
  if (m_pluginInfos.empty() && m_featureInfos.empty())
  {
    inspect();
  }
  std::string dir = dumpPath.substr(0, dumpPath.find_last_of("/"));
  if (!boost::filesystem::exists(dir)) {
    boost::filesystem::create_directories(dir);
  }
  {
    std::ofstream ofs(dumpPath);
    ofs << json();
    ofs.flush();
    ofs.close();
  }

  ExtensionLoader::get().getRegistryStampFilePath(true);
  return kAifOk;
}

} // namespace aif
