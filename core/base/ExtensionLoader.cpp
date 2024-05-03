#include <aif/base/ExtensionLoader.h>
#include <aif/log/Logger.h>

#include <aif/base/IPlugin.h>
#include <aif/base/IRegistration.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

namespace aif
{

t_aif_status ExtensionLoader::init(bool readFromDumpFile, std::string pluginPath)
{
  if (!std::filesystem::exists(pluginPath))
  {
    Loge("Plugin path does not exist: ", pluginPath);
    return kAifError;
  }

  if (initDone)
    return kAifOk;

  if (!RegistrationJob::get().getRegistrations(DEFAULT_EXTENSION_NAME).empty())
  {
    for (auto const &[id, reg] : RegistrationJob::get().getRegistrations(DEFAULT_EXTENSION_NAME))
    {
      t_aif_feature_info feature_info = {.name = id, .type = reg->getType(), .plugin_name = DEFAULT_EXTENSION_NAME};
      feature_infos.push_back(feature_info);
      reg->doRegister(); // This is needed for unittests (i.e. unittests link libraries directly and not via dlopen)
    }
  }

  if (readFromDumpFile)
  {
    if (!std::filesystem::exists(EDGEAI_VISION_PLUGIN_INFO_DUMP_PATH))
    {
      std::string cmd = EDGEAI_VISION_INSPECTOR_PATH;
      std::string arg = pluginPath;
      Logi("Executing command: ", cmd);
      pid_t pid = fork();
      if (pid == -1)
      {
        Logi("Failed to fork process");
        return kAifError;
      }
      else if (pid == 0)
      {
        char *args[] = {(char *)cmd.c_str(), (char *)arg.c_str(), NULL};
        execv(args[0], args);
        Logi("Failed to execute command: ", cmd + " " + arg);
        exit(1);
      }
      else
      {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
          Logi("Command exited with error status");
          return kAifError;
        }
      }
    }
    return initFromFile(EDGEAI_VISION_PLUGIN_INFO_DUMP_PATH);
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

  for (auto &plugin_info : plugin_infos)
  {
    if (!RegistrationJob::get().getRegistrations(plugin_info.name).empty())
    {
      for (auto const &[id, reg] : RegistrationJob::get().getRegistrations(plugin_info.name))
      {
        t_aif_feature_info feature_info = {.name = id, .type = reg->getType(), .plugin_name = plugin_info.name};
        feature_infos.push_back(feature_info);
      }
    }
  }

  unloadAllExtension();
  initDone = true;
  return kAifOk;
}

t_aif_status ExtensionLoader::initFromFile(std::string extensionInfoFilePath)
{
  std::ifstream ifs(extensionInfoFilePath);
  if (!ifs.is_open())
  {
    Logi("Failed to open file: ", extensionInfoFilePath);
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

  Logi("Successfully parsed json: ", json);
  for (auto &plugin : doc.GetArray())
  {
    t_aif_plugin_info plugin_info = {.name = plugin["name"].GetString(), .path = plugin["path"].GetString(), .handle = nullptr};
    plugin_infos.push_back(plugin_info);
    for (auto &feature : plugin["features"].GetArray())
    {
      t_aif_feature_info feature_info = {.name = feature["name"].GetString(), .type = stringToFeatureType(feature["type"].GetString()), .plugin_name = plugin["name"].GetString()};
      feature_infos.push_back(feature_info);
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

  plugin_infos.push_back(plugin_info);
  unsetenv("AIF_EXTENSION_NAME");
  return kAifOk;
}

t_aif_status ExtensionLoader::isAvailable(std::string feature_name, t_feature_type type)
{
  for (auto feature_info : feature_infos)
  {
    if (feature_info.name == feature_name && feature_info.type == type)
      return kAifOk;
  }
  return kAifError;
}

t_aif_status ExtensionLoader::enableFeature(std::string feature_name, t_feature_type type)
{
  if (isAvailable(feature_name, type) != kAifOk)
    return kAifError;

  for (auto feature_info : feature_infos)
  {
    if (feature_info.name != feature_name || feature_info.type != type)
      continue;

    std::string plugin_name = feature_info.plugin_name;
    for (auto plugin_info : plugin_infos)
    {
      if (plugin_info.name != plugin_name)
        continue;

      if (plugin_info.handle != nullptr)
        break;

      if (loadExtension(plugin_info.path) != kAifOk)
        return kAifError;

      break;
    }
    auto &reg = RegistrationJob::get().getRegistration(plugin_name, feature_name);
    if (reg)
      reg->doRegister();
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
  for (auto &plugin_info : plugin_infos)
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
  plugin_infos.clear();
  feature_infos.clear();
  initDone = false;
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
  return kUnknown;
}

} // namespace aif
