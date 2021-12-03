#include <aif/base/DelegateFactory.h>
#include <aif/log/Logger.h>

namespace aif {

t_aif_status DelegateFactory::registerGenerator(
        const std::string& id,
        const DelegateGenerator& delegateGenerator)
{
    try {
        if (m_delegateGenerators.find(id) != m_delegateGenerators.end()) {
            throw std::runtime_error(id + " delegate generator is already registered");
        }
    } catch (const std::exception& e) {
        Loge(__func__ , " Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }

    m_delegateGenerators[id] = delegateGenerator;
    return kAifOk;
}

std::shared_ptr<Delegate> DelegateFactory::getDelegate(const std::string& id, const std::string& option)
{
    try {
        if (m_delegates.find(id) == m_delegates.end() &&
            m_delegateGenerators.find(id) == m_delegateGenerators.end()) {
            throw std::runtime_error(id + " delegate generator is not registered");
        }
        if (m_delegates.find(id) != m_delegates.end()) {
            return m_delegates[id];
        }
        m_delegates[id] = m_delegateGenerators[id](option);
        return m_delegates[id];

    } catch (const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return nullptr;
    } catch (...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return nullptr;
    }
}

} // end of idspace aif
