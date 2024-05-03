#include "ExtensionInspector.h"

#include <cstring>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
  try
  {
    std::string pluginPath = "";
    if (argc > 1)
    {
      if (!std::strcmp(argv[1], "-h") || !std::strcmp(argv[1], "--help"))
      {
        std::cout << "Usage: edgeai-inspector <edgeai-extension-path>" << std::endl;
        return 0;
      }
      pluginPath = argv[1];
    }
    if (pluginPath.empty())
    {
      pluginPath = "/usr/lib/edgeai-extensions";
    }
    std::cout << "Inspecting path: " << pluginPath << std::endl;
    aif::ExtensionInspector::get().setPluginPath(pluginPath);
    if (aif::ExtensionInspector::get().inspect() != aif::kAifOk)
    {
      std::cerr << "Error: Failed to inspect plugins" << std::endl;
      return 1;
    }
    std::string dumpJson = aif::ExtensionInspector::get().json();
    std::cout << dumpJson << std::endl;
    if (!dumpJson.empty())
    {
      std::ofstream ofs(EDGEAI_VISION_PLUGIN_INFO_DUMP_PATH);
      ofs << dumpJson;
      ofs.close();
    }
    std::cout << "Dumped plugin info to: " << EDGEAI_VISION_PLUGIN_INFO_DUMP_PATH << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}