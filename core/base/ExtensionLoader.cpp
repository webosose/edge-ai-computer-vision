/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/ExtensionLoader.h>
#include <aif/log/Logger.h>
#include <aif/tools/ProcessRunner.h>

#include <aif/base/IPlugin.h>
#include <aif/base/IRegistration.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#include <boost/filesystem.hpp>
#include <chrono>
#include <dlfcn.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <regex>
#include <sstream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

namespace fs = boost::filesystem;

namespace aif
{

t_aif_status ExtensionLoader::initRetry(bool readRegistryFile,
  const std::string& extensionDirectoryPath, const std::vector<std::string>& allowedExtensionNames, bool forceRetry) noexcept
{
  t_aif_status status = kAifError;
  status = init(readRegistryFile, extensionDirectoryPath, allowedExtensionNames);
  while ((forceRetry == true && --m_retryCount > 0)
          || (status != kAifOk && --m_retryCount > 0)) {
    Logi("Retry count: ", m_retryCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    status = init(readRegistryFile, extensionDirectoryPath, allowedExtensionNames);
  }

  Logi("Fault tolerance: ", getFaultTolerance() ? "true" : "false");
  if (readRegistryFile && status != kAifOk && getFaultTolerance() == true){
    Logi("Failed to read registry file, regeneration as well. Trying to load all extensions");
    status = init(false, extensionDirectoryPath, allowedExtensionNames);
  }
  return status;
}

t_aif_status ExtensionLoader::init(bool readRegistryFile,
  const std::string& extensionDirectoryPath, const std::vector<std::string>& allowedExtensionNames) noexcept
{
  try {
    if (setExtensionDirectory(extensionDirectoryPath) != kAifOk)
      return kAifError;

    if (setAllowedExtensionNames(allowedExtensionNames) != kAifOk)
      return kAifError;

    if (isInitialized())
      return kAifOk;

    if (!RegistrationJob::get().getRegistrations(DEFAULT_EXTENSION_NAME).empty())
    {
      for (auto const &[id, reg] : RegistrationJob::get().getRegistrations(DEFAULT_EXTENSION_NAME))
      {
        //Logd("Feature: ", id, ", type: ", featureTypeToString(reg->getType()), ", pluginName: ", DEFAULT_EXTENSION_NAME);
        t_aif_feature_info feature_info = {.name = id, .type = reg->getType(), .pluginName = DEFAULT_EXTENSION_NAME};
        m_featureInfos.push_back(std::move(feature_info));
        reg->doRegister(); // This is needed for unittests (i.e. unittests link libraries directly and not via dlopen)
      }
    }

    if (readRegistryFile)
    {
      if (isNeededToGenRegistryFile())
      {
        if (runInspector() != kAifOk)
          return kAifError;
      }
      if (initFromRegistryFile() != kAifOk)
        return kAifError;

      m_initDone = true;
      return kAifOk;
    }

    fs::directory_iterator itr(m_extensionDirectoryPath);
    for (; itr != fs::directory_iterator(); ++itr)
    {
      if (!fs::is_regular_file(itr->path()) ||
          fs::is_symlink(itr->path()) ||
          itr->path().filename().string().find("libedgeai-") != 0)
      {
        continue;
      }
      Logi("m_extensionDirectoryPath:", m_extensionDirectoryPath, ", itr->path:", itr->path().c_str());
      if (loadExtension(itr->path().c_str()) != kAifOk) {
        Loge("loadExtension() failed.", "m_extensionDirectoryPath:", m_extensionDirectoryPath, ", itr->path:", itr->path().c_str());
      }
    }

    for (auto &plugin_info : m_pluginInfos)
    {
      if (!RegistrationJob::get().getRegistrations(plugin_info.name).empty())
      {
        for (auto const &[id, reg] : RegistrationJob::get().getRegistrations(plugin_info.name))
        {
          //Logd("Feature: ", id, ", type: ", featureTypeToString(reg->getType()), ", pluginName: ", plugin_info.name);
          t_aif_feature_info feature_info = {.name = id, .type = reg->getType(), .pluginName = plugin_info.name};
          m_featureInfos.push_back(std::move(feature_info));
        }
      }
    }

    unloadAllExtension();
    m_initDone = true;
    return kAifOk;
  } catch (const fs::filesystem_error& e) {
      Loge("Exception occurred while iterating directory: ", e.what());
      return kAifError;
  } catch (const std::exception& e) {
      Loge("Exception occurred: ", e.what());
      return kAifError;
  } catch (...) {
      Loge("Caught unknown exception");
      return kAifError;
  }
}

t_aif_status ExtensionLoader::initFromRegistryFile()
{
  Logi("Reading registry file: ", m_registryFilePath);
  std::stringstream buffer;
  {
    std::ifstream ifs(m_registryFilePath);
    if (!ifs.is_open())
    {
      Loge("Failed to open file: ", m_registryFilePath);
      return kAifError;
    }
    // buffer << ifs.rdbuf(); //FIXME: If the below code fixes EXP60-CPP, remove this line
    std::string line;
    while (std::getline(ifs, line)) {
        buffer << line << '\n';
    }
    ifs.close();
  }
  std::string json = buffer.str();
  rapidjson::Document doc;
  doc.Parse(json.c_str());
  if (doc.HasParseError())
  {
    Loge("Failed to parse json: ", json);
    return kAifError;
  }

  Logd("Successfully parsed json: ", json);
  for (const auto &plugin : doc.GetArray())
  {
    if (!isAllowedExtension(plugin["name"].GetString())) {
      Logi("Skipping plugin: ", plugin["name"].GetString(), " because it is not allowed");
      continue;
    }

    t_aif_plugin_info plugin_info = {
      .name = plugin["name"].GetString(),
      .path = plugin["path"].GetString(),
      .handle = nullptr
    };
    //Optional fields in registry file
    std::map<std::string, std::string*> plugin_info_map = {
        {"alias", &plugin_info.alias},
        {"version", &plugin_info.version},
        {"description", &plugin_info.description}
    };
    auto setOptionalValues = [&plugin, &plugin_info_map] (const std::string& key) {
      if (key.empty() || plugin_info_map.find(key) == plugin_info_map.end())
        return;
      if (plugin.HasMember(key.c_str()) && plugin[key.c_str()].IsString()) {
        std::string value = plugin[key.c_str()].GetString()? plugin[key.c_str()].GetString() : "";
        *plugin_info_map[key] = std::move(value);
      }
    };
    setOptionalValues("alias");
    setOptionalValues("version");
    setOptionalValues("description");
    m_pluginInfos.push_back(std::move(plugin_info));

    //Required fields in registry file
    t_aif_feature_info feature_info;
    for (const auto &feature : plugin["features"].GetArray())
    {
      auto hasRequired = [&feature] () {
        if (!feature.HasMember("name") || !feature["name"].IsString() ||
            !feature.HasMember("type") || !feature["type"].IsString())
          return false;
        return true;
      };
      if (!hasRequired())
        continue;

      t_aif_feature_info feature_info = {.name = feature["name"].GetString(), .type = stringToFeatureType(feature["type"].GetString()), .pluginName = plugin["name"].GetString()};
      m_featureInfos.push_back(std::move(feature_info));
    }
  }
  return kAifOk;
}

t_aif_status ExtensionLoader::loadExtension(const std::string& extensionFilePath)
{
  std::string fileName =  fs::path(extensionFilePath).filename().string();
  std::string extensionName = getBaseFileName(fileName);
  setenv("AIF_EXTENSION_NAME", extensionName.c_str(), 1);
  void *handle = dlopen(extensionFilePath.c_str(), RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND);
  if (handle == nullptr)
  {
    unsetenv("AIF_EXTENSION_NAME");
    Loge("Failed to dlopen extension: ", extensionFilePath, ", ", dlerror());
    return kAifError;
  }
  t_aif_plugin_info plugin_info = {.name = extensionName, .path = extensionFilePath, .handle = handle};

  typedef IPlugin *(*plugin_init_func)();
  plugin_init_func plugin_init = (plugin_init_func)dlsym(handle, "plugin_init");
  if (plugin_init == nullptr)
  {
    Loge("Failed to find plugin_init function: ", dlerror());
    dlclose(handle);
    return kAifError;
  }
  auto plugin = plugin_init();
  if (plugin == nullptr)
  {
    Loge("Failed to call plugin_init function: ", dlerror());
    dlclose(handle);
    return kAifError;
  }
  if (!plugin->getAlias().empty())
  {
    plugin_info.alias = plugin->getAlias();
    Logd("Loaded plugin alias: ", plugin->getAlias());
  }
  if (!plugin->getVersion().empty())
  {
    plugin_info.version = plugin->getVersion();
    Logd("Loaded plugin version: ", plugin->getVersion());
  }
  if (!plugin->getDescription().empty())
  {
    plugin_info.description = plugin->getDescription();
    Logd("Loaded plugin description: ", plugin->getDescription());
  }
  for (const auto& hiddenFeature : plugin->getHiddenFeatures())
  {
    Logd("Hidden feature: ", hiddenFeature);
    RegistrationJob::get().removeRegistration(extensionName, hiddenFeature);
  }

  m_pluginInfos.push_back(std::move(plugin_info));
  unsetenv("AIF_EXTENSION_NAME");
  return kAifOk;
}

bool ExtensionLoader::isFeatureNameHasPluginAlias(const std::string& featureName)
{
  return featureName.find(FEATURE_NAME_DELIMITER) != std::string::npos;
}

t_aif_parsed_feature_name ExtensionLoader::parseFeatureName(const std::string& featureName)
{
  t_aif_parsed_feature_name parsedFeatureName;
  size_t pos = featureName.find(FEATURE_NAME_DELIMITER);
  if (pos == std::string::npos)
  {
    parsedFeatureName.pluginAlias = "";
    parsedFeatureName.featureName = featureName;
  }
  else
  {
    parsedFeatureName.pluginAlias = featureName.substr(0, pos);
    parsedFeatureName.featureName = featureName.substr(pos + 2);
  }
  return parsedFeatureName;
}

std::string ExtensionLoader::getPluginNameByAlias(const std::string& alias)
{
  for (const auto& plugin_info : m_pluginInfos)
  {
    if (plugin_info.alias == alias)
      return plugin_info.name;
  }
  return "";
}

std::string ExtensionLoader::getPluginAliasByName(const std::string& name)
{
  for (const auto& plugin_info : m_pluginInfos)
  {
    if (plugin_info.name == name)
      return plugin_info.alias;
  }
  return "";
}

t_aif_status ExtensionLoader::isAvailable(const std::string& featureName, t_feature_type type, const std::string& pluginName)
{
  if (isFeatureNameHasPluginAlias(featureName))
  {
    const auto& parsedFeatureName = parseFeatureName(featureName);
    const auto& parsedPluginName = getPluginNameByAlias(parsedFeatureName.pluginAlias);
    if (parsedPluginName.empty())
    {
      Loge("Plugin alias not found: ", parsedFeatureName.pluginAlias);
      return kAifError;
    }
    return isAvailable(parsedFeatureName.featureName, type, parsedPluginName);
  }

  for (const auto& feature_info : m_featureInfos)
  {
    if (feature_info.name == featureName && feature_info.type == type)
    {
      if (pluginName.empty() || feature_info.pluginName == pluginName)
        return kAifOk;
    }
  }

  Loge("Feature not found: ", featureName, ", type: ", featureTypeToString(type), ", pluginName: ", pluginName);
  return kAifError;
}

t_aif_status ExtensionLoader::enableFeature(const std::string& featureName, t_feature_type type, const std::string& pluginName)
{
  if (isAvailable(featureName, type) != kAifOk)
    return kAifError;

  if (isFeatureNameHasPluginAlias(featureName)) {
    const auto& parsedFeatureName = parseFeatureName(featureName);
    const auto& parsedPluginName = getPluginNameByAlias(parsedFeatureName.pluginAlias);
    if (parsedPluginName.empty())
    {
      Loge("Plugin alias not found: ", parsedFeatureName.pluginAlias);
      return kAifError;
    }
    return enableFeature(parsedFeatureName.featureName, type, parsedPluginName);
  }

  for (const auto& feature_info : m_featureInfos)
  {
    if (feature_info.name != featureName || feature_info.type != type)
      continue;

    const std::string& pName = feature_info.pluginName;
    if (!pluginName.empty() && pName != pluginName)
      continue;

    for (const auto& plugin_info : m_pluginInfos)
    {
      if (plugin_info.name != pName)
        continue;

      if (plugin_info.handle != nullptr)
        break;

      if (loadExtension(plugin_info.path) != kAifOk)
        return kAifError;

      break;
    }
    const auto &reg = RegistrationJob::get().getRegistration(pName, featureName);
    if (reg) {
      std::string pluginAlias = getPluginAliasByName(pluginName);
      Logd("pluginAlias: ", pluginAlias);
      if (pluginAlias.empty())
        reg->doRegister();
      else
        reg->doRegister(pluginAlias + FEATURE_NAME_DELIMITER);
    } else {
      Loge("Failed to get registration for feature: ", featureName, ", pluginName: ", pluginName);
    }

    break;
  }

  return kAifOk;
}

ExtensionLoader::~ExtensionLoader()
{
  unloadAllExtension();
  clear();
}

t_aif_status ExtensionLoader::unloadAllExtension()
{
  for (auto& plugin_info : m_pluginInfos)
  {
    if (plugin_info.handle != nullptr)
    {
      // Doesn't guarantee that the plugin is unloaded from process memory space
      dlclose(plugin_info.handle);
      plugin_info.handle = nullptr;
    }
  }
  return kAifOk;
}

t_aif_status ExtensionLoader::clear()
{
  m_pluginInfos.clear();
  m_featureInfos.clear();
  m_initDone = false;
  return kAifOk;
}

std::string ExtensionLoader::featureTypeToString(t_feature_type type)
{
  switch (type)
  {
  case kDetector:
    return "Detector";
  case kNodeOperation:
    return "NodeOperation";
  case kPipeDescriptor:
    return "PipeDescriptor";
  case kSolution:
    return "Solution";
  default:
    return "Unknown";
  }
}

t_feature_type ExtensionLoader::stringToFeatureType(const std::string& type)
{
  if (type == "Detector")
    return kDetector;
  if (type == "NodeOperation")
    return kNodeOperation;
  if (type == "PipeDescriptor")
    return kPipeDescriptor;
  if (type == "Solution")
    return kSolution;
  return kUnknown;
}

t_aif_status ExtensionLoader::setExtensionDirectory(const std::string& extensionDirectoryPath)
{
  if (!isReadableDirectory(extensionDirectoryPath))
    return kAifError;

  m_extensionDirectoryPath = extensionDirectoryPath;
  return kAifOk;
}

t_aif_status ExtensionLoader::setAllowedExtensionNames(const std::vector<std::string>& allowedExtensionNames)
{
  if (allowedExtensionNames.empty())
  {
    m_allowAllExtensions = true;
    return kAifOk;
  }

  m_allowAllExtensions = false;
  m_allowedExtensionNames = allowedExtensionNames;
  return kAifOk;
}

bool ExtensionLoader::isAllowedExtension(const std::string& extensionName)
{
  if (m_allowAllExtensions)
    return true;

  for (const auto& allowedExtensionName : m_allowedExtensionNames)
  {
    if (compareExtensionName(extensionName, allowedExtensionName))
      return true;
  }
  return false;
}

bool ExtensionLoader::isNeededToGenRegistryFile() noexcept
{
  try {
    // 1. Check if registry file exists and is readable
    {
      std::stringstream buffer;
      {
        std::ifstream ifs(m_registryFilePath);
        if (!ifs.is_open())
          return true;
        std::string line;
        while (std::getline(ifs, line)) {
            buffer << line << '\n';
        }
        ifs.close();
      }
      std::string json = buffer.str();
      rapidjson::Document doc;
      doc.Parse(json.c_str());
      if (doc.HasParseError())
      {
        Loge("Failed to parse json: ", json);
        fs::remove(m_registryFilePath);
        return true;
      }
    }

    // 2. Check if stamp file exists
    {
      std::string registryStampFilePath = getRegistryStampFilePath(false);
      if (registryStampFilePath.empty())
        return true;

      if (!fs::exists(registryStampFilePath))
      {
        fs::remove(m_registryFilePath);
        std::string dir = m_registryFilePath.substr(0, m_registryFilePath.find_last_of("/"));
        if(isReadableDirectory(dir)) {
          fs::directory_iterator itr(dir);
          for (; itr != fs::directory_iterator(); ++itr)
          {
            if (!fs::exists(itr->path())) {
              continue;
            }
            if (itr->path().string().find(m_registryFilePath+".done.") == 0)
            {
              fs::remove(itr->path());
            }
          }
        }
        return true;
      }
    }
    return false;
  } catch (const fs::filesystem_error& e) {
    //Although exception occurs, the file should be re-gen.
    Loge("Exception occurred while iterating directory: ", e.what());
    return true;
  } catch (const std::exception& e) {
    Loge("Exception occurred: ", e.what());
    return true;
  } catch (...) {
    Loge("Caught unknown exception while iterating directory");
    return true;
  }
}

std::string ExtensionLoader::getRegistryStampFilePath(bool create) {
  std::initializer_list<std::string> args = {"OSInfo", "query", "--format=json", "webos_release", "webos_build_id"};
  ProcessRunner processRunner(NYX_CMD_PATH, args);
  if (processRunner.getExitCode() != 0)
  {
    Loge("Failed to get webos_release and webos_build_id");
    return "";
  }
  std::string output = processRunner.getResult();

  rapidjson::Document doc;
  doc.Parse(output.c_str());
  if (doc.HasParseError())
  {
    Loge("Failed to parse json: ", output);
    return "";
  }
  std::string webosReleaseVersion = doc["webos_release"].GetString();
  std::string webosBuildId = doc["webos_build_id"].GetString();
  Logi("webosReleaseVersion: ", webosReleaseVersion, ", webosBuildId: ", webosBuildId);
  std::string stampExtensionInfoFilePath = m_registryFilePath + ".done." + webosReleaseVersion + "-" + webosBuildId;
  if (!create) {
    return stampExtensionInfoFilePath;
  }

  std::ofstream ofs(stampExtensionInfoFilePath);
  ofs.close();
  return stampExtensionInfoFilePath;
}

t_aif_status ExtensionLoader::runInspector()
{
  ProcessRunner processRunner(EDGEAI_VISION_INSPECTOR_PATH, {m_extensionDirectoryPath});
  if (processRunner.getExitCode() != 0)
  {
    Loge("Failed to run inspector");
    return kAifError;
  }
  std::string output = processRunner.getResult();
  Logd("Inspector output: ", output);
  return kAifOk;
}

std::string ExtensionLoader::getBaseFileName(const std::string& name) noexcept
{
  try {
    std::regex so_regex(R"(\.so.*$)");
    return std::regex_replace(name, so_regex, "");
  } catch (const std::exception& e) {
    Loge("Exception occurred: ", e.what());
    return name;
  }
}

bool ExtensionLoader::compareExtensionName(const std::string& name1, const std::string& name2)
{
  if (name1.empty() || name2.empty())
    return false;

  std::string name1_ = name1;
  std::string name2_ = name2;
  if (name1_.find("lib") == 0)
    name1_ = name1_.substr(3);
  if (name2_.find("lib") == 0)
    name2_ = name2_.substr(3);

  std::string baseName1 = getBaseFileName(name1_);
  std::string baseName2 = getBaseFileName(name2_);
  return baseName1 == baseName2;
}

bool ExtensionLoader::isReadableDirectory(const std::string& path)
{
  if (!fs::exists(path) || !fs::is_directory(path)) {
    Loge("Directory does not exist or is not readable: ", path);
    return false;
  }
  return true;
}

} // namespace aif
