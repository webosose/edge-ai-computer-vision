/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ExtensionInspector.h"

#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>


static void help() noexcept
{
  try {
    std::cout << "Usage: edgeai-inspector <edgeai-extension-path>" << std::endl;
  } catch(const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  } catch(...) {
    std::cerr << "Caught unknown exception" << std::endl;
  }
}

static int inspectPlugins(const std::string& pluginPath) noexcept {
  try {
    aif::ExtensionInspector::get().setPluginPath(pluginPath);
    if (aif::ExtensionInspector::get().inspect() != aif::kAifOk) {
      std::cerr << "Error: Failed to inspect plugins" << std::endl;
      return 1;
    }
    aif::ExtensionInspector::get().dump(EDGEAI_VISION_EXTENSION_REGISTRY_PATH);
    std::cout << "Dumped plugin info to: " << EDGEAI_VISION_EXTENSION_REGISTRY_PATH << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;
    return 1;
  }
}

int main(int argc, char *argv[]) noexcept
{
  std::string pluginPath = EDGEAI_VISION_EXTENSION_PATH;
  if (argc > 1)
  {
    if (!std::strcmp(argv[1], "-h") || !std::strcmp(argv[1], "--help"))
    {
      help();
      return 0;
    }
    pluginPath = argv[1];
  }
  return inspectPlugins(pluginPath);
}
