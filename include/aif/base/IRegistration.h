#pragma once

#include <aif/base/Types.h>

#include <string>
#include <map>
#include <memory>

#define DEFAULT_EXTENSION_NAME "core"

namespace aif
{

  class IRegistration
  {
  public:
    virtual void doRegister() = 0;
    virtual ~IRegistration() {}
    std::string getId() { return m_id; }
    t_feature_type getType() { return m_type; }
    std::string getExtensionName() { return m_extension; }

  protected:
    IRegistration(const std::string &id, const t_feature_type type) : m_id(id), m_type(type)
    {
      const char *extName = std::getenv("AIF_EXTENSION_NAME");
      if (!extName)
      {
        extName = DEFAULT_EXTENSION_NAME;
      }
      m_extension = std::string(extName);
    }

  protected:
    std::string m_id;
    t_feature_type m_type;
    std::string m_extension;
  };

  class RegistrationJob
  {
  public:
    static RegistrationJob &get()
    {
      static RegistrationJob instance;
      return instance;
    }

    void addRegistration(std::string extensionName, std::string id, IRegistration *registration)
    {
      m_registrations[extensionName][id] = std::move(registration);
    }
    void removeRegistration(std::string extensionName, std::string id)
    {
      if (m_registrations[extensionName].find(id) != m_registrations[extensionName].end())
      {
        m_registrations[extensionName].erase(id);
      }
    }

    IRegistration *&getRegistration(std::string extensionName, std::string id)
    {
      return m_registrations[extensionName][id];
    }

    std::map<std::string, IRegistration *> &getRegistrations(std::string extensionName)
    {
      return m_registrations[extensionName];
    }

  private:
    using regInstanceMap = std::map<std::string, IRegistration *>;
    std::map<std::string, regInstanceMap> m_registrations;
  };

} // end of namespace aif