/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include <aif/base/IPlugin.h>

namespace aif {

/**
 * @brief Plugin class is for describing a plugin.
 * 
 */
class Plugin : public IPlugin
{
public:
  /**
   * @brief Construct a new Plugin object
   */

  Plugin() {}
  /**
   * @brief Destroy the Plugin object
   */
  ~Plugin() {}

  /**
   * @brief Add a feature to be not exposed.
   * @param featureName
   */
  void makeHidden(std::string featureName) { m_hiddenFeatures.insert(featureName); }
  /**
   * @brief Set the plugin alias name
   * @param alias
   */
  void setAlias(std::string alias) { m_alias = alias; }

  /**
   * @brief Set the plugin version
   * @param version
   */
  void setVersion(std::string version)  { m_version = version; }

  /**
   * @brief Set the plugin description
   * @param description
   */
  void setDescription(std::string description) { m_description = description; }
};

} // namespace aif

extern "C" {
  aif::Plugin* plugin_init();
}
