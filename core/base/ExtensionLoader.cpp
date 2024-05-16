#include <aif/base/ExtensionLoader.h>
#include <aif/log/Logger.h>
#include <aif/tools/ProcessRunner.h>

#include <aif/base/IPlugin.h>
#include <aif/base/IRegistration.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

namespace aif
{

t_aif_status ExtensionLoader::init(bool readRegistryFile, std::string pluginPath, std::vector<std::string> allowedExtensionNames)
{
  if (!std::filesystem::exists(pluginPath))
  {
    Loge("Plugin path does not exist: ", pluginPath);
    return kAifError;
  }
  setAllowedExtensionNames(allowedExtensionNames);

  if (m_initDone)
    return kAifOk;

  if (!RegistrationJob::get().getRegistrations(DEFAULT_EXTENSION_NAME).empty())
  {
    for (auto const &[id, reg] : RegistrationJob::get().getRegistrations(DEFAULT_EXTENSION_NAME))
    {
      t_aif_feature_info feature_info = {.name = id, .type = reg->getType(), .pluginName = DEFAULT_EXTENSION_NAME};
      Logd("Feature: ", id, ", type: ", featureTypeToString(reg->getType()), ", pluginName: ", DEFAULT_EXTENSION_NAME);
      m_featureInfos.push_back(feature_info);
      reg->doRegister(); // This is needed for unittests (i.e. unittests link libraries directly and not via dlopen)
    }
  }

  if (readRegistryFile)
  {
    if (isNeededToGenRegistryFile())
    {
      if (runInspector(pluginPath) != kAifOk)
        return kAifError;
    }
    return initFromRegistryFile();
  }

  for (const auto &entry : std::filesystem::directory_iterator(pluginPath))
  {
    Logi("ExtensionLoader::init()", "pluginPath:", pluginPath, ", entry.path:", entry.path().c_str());
    if (entry.is_regular_file() && entry.path().extension() == ".so" &&
        entry.path().filename().string().find("libedgeai-") == 0)
    {
      if (loadExtension(entry.path().c_str()) != kAifOk)
        continue;
    }
  }

  for (auto &plugin_info : m_pluginInfos)
  {
    if (!RegistrationJob::get().getRegistrations(plugin_info.name).empty())
    {
      for (auto const &[id, reg] : RegistrationJob::get().getRegistrations(plugin_info.name))
      {
        t_aif_feature_info feature_info = {.name = id, .type = reg->getType(), .pluginName = plugin_info.name};
        Logd("Feature: ", id, ", type: ", featureTypeToString(reg->getType()), ", pluginName: ", plugin_info.name);
        m_featureInfos.push_back(feature_info);
      }
    }
  }

  unloadAllExtension();
  m_initDone = true;
  return kAifOk;
}

t_aif_status ExtensionLoader::initFromRegistryFile()
{
  Logi("Reading registry file: ", m_registryFilePath);
  std::ifstream ifs(m_registryFilePath);
  if (!ifs.is_open())
  {
    Logi("Failed to open file: ", m_registryFilePath);
    return kAifError;
  }
  std::stringstream buffer;
  buffer << ifs.rdbuf();
  ifs.close();
  std::string json = buffer.str();
  rapidjson::Document doc;
  doc.Parse(json.c_str());
  if (doc.HasParseError())
  {
    Logi("Failed to parse json: ", json);
    return kAifError;
  }

  Logd("Successfully parsed json: ", json);
  for (auto &plugin : doc.GetArray())
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
    if (plugin.HasMember("alias"))
      plugin_info.alias = plugin["alias"].GetString();
    if (plugin.HasMember("version"))
      plugin_info.version = plugin["version"].GetString();
    if (plugin.HasMember("description"))
      plugin_info.description = plugin["description"].GetString();
    m_pluginInfos.push_back(plugin_info);

    for (auto &feature : plugin["features"].GetArray())
    {
      t_aif_feature_info feature_info = {.name = feature["name"].GetString(), .type = stringToFeatureType(feature["type"].GetString()), .pluginName = plugin["name"].GetString()};
      m_featureInfos.push_back(feature_info);
    }
  }
  return kAifOk;
}

t_aif_status ExtensionLoader::loadExtension(std::string extensionFilePath)
{
  std::string extensionName = std::filesystem::path(extensionFilePath).filename().string();
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
    return kAifError;
  }
  auto plugin = plugin_init();
  if (plugin == nullptr)
  {
    Loge("Failed to call plugin_init function: ", dlerror());
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
  for (auto hiddenFeature : plugin->getHiddenFeatures())
  {
    Logd("Hidden feature: ", hiddenFeature);
    RegistrationJob::get().removeRegistration(extensionName, hiddenFeature);
  }

  m_pluginInfos.push_back(plugin_info);
  unsetenv("AIF_EXTENSION_NAME");
  return kAifOk;
}

bool ExtensionLoader::isFeatureNameHasPluginAlias(std::string featureName)
{
  return featureName.find(FEATURE_NAME_DELIMITER) != std::string::npos;
}

t_aif_parsed_feature_name ExtensionLoader::parseFeatureName(std::string featureName)
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

std::string ExtensionLoader::getPluginNameByAlias(std::string alias)
{
  for (auto plugin_info : m_pluginInfos)
  {
    if (plugin_info.alias == alias)
      return plugin_info.name;
  }
  return "";
}

std::string ExtensionLoader::getPluginAliasByName(std::string name)
{
  for (auto plugin_info : m_pluginInfos)
  {
    if (plugin_info.name == name)
      return plugin_info.alias;
  }
  return "";
}

t_aif_status ExtensionLoader::isAvailable(std::string featureName, t_feature_type type, std::string pluginName)
{
  if (isFeatureNameHasPluginAlias(featureName))
  {
    auto parsedFeatureName = parseFeatureName(featureName);
    auto pluginName = getPluginNameByAlias(parsedFeatureName.pluginAlias);
    if (pluginName.empty())
    {
      Loge("Plugin alias not found: ", parsedFeatureName.pluginAlias);
      return kAifError;
    }
    return isAvailable(parsedFeatureName.featureName, type, pluginName);
  }

  for (auto feature_info : m_featureInfos)
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

t_aif_status ExtensionLoader::enableFeature(std::string featureName, t_feature_type type, std::string pluginName)
{
  if (isAvailable(featureName, type) != kAifOk)
    return kAifError;

  if (isFeatureNameHasPluginAlias(featureName)) {
    auto parsedFeatureName = parseFeatureName(featureName);
    auto pluginName = getPluginNameByAlias(parsedFeatureName.pluginAlias);
    if (pluginName.empty())
    {
      Loge("Plugin alias not found: ", parsedFeatureName.pluginAlias);
      return kAifError;
    }
    return enableFeature(parsedFeatureName.featureName, type, pluginName);
  }

  for (auto feature_info : m_featureInfos)
  {
    if (feature_info.name != featureName || feature_info.type != type)
      continue;

    std::string pName = feature_info.pluginName;
    if (!pluginName.empty() && pName != pluginName)
      continue;

    for (auto plugin_info : m_pluginInfos)
    {
      if (plugin_info.name != pName)
        continue;

      if (plugin_info.handle != nullptr)
        break;

      if (loadExtension(plugin_info.path) != kAifOk)
        return kAifError;

      break;
    }
    auto &reg = RegistrationJob::get().getRegistration(pName, featureName);
    if (reg) {
      std::string pluginAlias = getPluginAliasByName(pluginName);
      Logd("pluginAlias: ", pluginAlias);
      if (pluginAlias.empty())
        reg->doRegister();
      else
        reg->doRegister(pluginAlias + FEATURE_NAME_DELIMITER);
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
  for (auto &plugin_info : m_pluginInfos)
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

t_feature_type ExtensionLoader::stringToFeatureType(std::string type)
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

t_aif_status ExtensionLoader::setAllowedExtensionNames(std::vector<std::string> allowedExtensionNames)
{
  if (allowedExtensionNames.empty())
  {
    m_allowAllExtensions = true;
    return kAifOk;
  }

  for (auto &extensionName : allowedExtensionNames)
  {
    if (extensionName.empty())
    {
      Loge("Empty extension name is not allowed");
      continue;
    }
    if (extensionName.find("lib") != 0)
    {
      extensionName = "lib" + extensionName;
    }
    if (extensionName.find(".so") != extensionName.size() - 3)
    {
      extensionName = extensionName + ".so";
    }
  }

  m_allowAllExtensions = false;
  m_allowedExtensionNames = allowedExtensionNames;
  return kAifOk;
}

bool ExtensionLoader::isAllowedExtension(std::string extensionName)
{
  if (m_allowAllExtensions)
    return true;

  for (auto &allowedExtensionName : m_allowedExtensionNames)
  {
    if (extensionName == allowedExtensionName)
      return true;
  }
  return false;
}

bool ExtensionLoader::isNeededToGenRegistryFile()
{
  if (!std::filesystem::exists(m_registryFilePath))
    return true;

  std::string registryStampFilePath = getRegistryStampFilePath(false);
  if (registryStampFilePath.empty())
    return true;

  if (!std::filesystem::exists(registryStampFilePath))
  {
    std::string dir = m_registryFilePath.substr(0, m_registryFilePath.find_last_of("/"));
    for (const auto &entry : std::filesystem::directory_iterator(dir))
    {
      if (entry.path().string().find(m_registryFilePath+".done.") == 0)
      {
        std::filesystem::remove(entry.path());
      }
    }
    std::filesystem::remove(m_registryFilePath);
    return true;
  }

  return false;
}

std::string ExtensionLoader::getRegistryStampFilePath(bool create) {
  std::initializer_list<std::string> args = {"OSInfo", "query", "--format=json", "webos_release", "webos_build_id"};
  ProcessRunner processRunner(NYX_CMD_PATH, args);
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

t_aif_status ExtensionLoader::runInspector(std::string pluginPath)
{
  std::string arg = pluginPath;
  ProcessRunner processRunner(EDGEAI_VISION_INSPECTOR_PATH, {pluginPath});
  std::string output = processRunner.getResult();
  Logd("Inspector output: ", output);
  return kAifOk;
}

} // namespace aif
