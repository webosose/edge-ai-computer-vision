#pragma once

#include <string>
#include <set>

namespace aif {

class IPlugin
{
public:
  virtual std::string getAlias() const { return m_alias; }
  virtual std::string getVersion() const { return m_version; }
  virtual std::string getDescription() const { return m_description; }
  virtual std::set<std::string> getHiddenFeatures() const { return m_hiddenFeatures; }

protected:
  std::string m_alias;
  std::string m_version;
  std::string m_description;
  std::set<std::string> m_hiddenFeatures;
  virtual ~IPlugin() {}
};

} // namespace aif