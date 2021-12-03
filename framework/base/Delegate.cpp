#include <aif/base/Delegate.h>

namespace aif {

Delegate::Delegate(const::std::string& name, const std::string& option)
    : m_name(name)
    , m_option(option)
{
}

Delegate::~Delegate()
{
}

const std::string& Delegate::getName() const
{
    return m_name;
}

}   // end of namespace aif
