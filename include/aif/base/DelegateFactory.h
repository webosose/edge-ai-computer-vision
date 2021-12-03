#ifndef AIF_DELEGATE_FACTORY_H
#define AIF_DELEGATE_FACTORY_H

#include <aif/base/Types.h>
#include <aif/base/Delegate.h>

#include <string>
#include <unordered_map>

namespace aif {

using DelegateGenerator = std::shared_ptr<Delegate>(*)(const std::string&);

class DelegateFactory
{
public:
    static DelegateFactory& get() {
        static DelegateFactory instance;
        return instance;
    }
    t_aif_status registerGenerator(
            const std::string& id,
            const DelegateGenerator& DelegateGenerator);
    std::shared_ptr<Delegate> getDelegate(const std::string& id, const std::string& options = "");
    void clear() {
        m_delegates.clear();
    }

private:
    DelegateFactory() {}
    DelegateFactory(const DelegateFactory&) {}
    ~DelegateFactory() {}

private:
    std::unordered_map<std::string, std::shared_ptr<Delegate>> m_delegates;
    std::unordered_map<std::string, DelegateGenerator> m_delegateGenerators;
};

} // end of idspace aif

#endif  // AIF_DELEGATE_FACTORY_H
