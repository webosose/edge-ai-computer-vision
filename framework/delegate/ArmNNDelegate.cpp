#include <aif/delegate/ArmNNDelegate.h>

namespace aif {

ArmNNDelegate::ArmNNDelegate()
    : m_delegateOptions(armnn::Compute::CpuRef)
{
}

ArmNNDelegate::~ArmNNDelegate()
{
}

std::vector<std::string> ArmNNDelegate::splitString(
                                        const std::string& str,
                                        const char delim)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ss(str);
    while (std::getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

t_aif_status ArmNNDelegate::setArmNNDelegateOptions(
                            const std::string& armnnOptions)
{
    if (armnnOptions.empty()) {
        std::cout << "ArmnnOptions empty, Apply default options...\n";
        return kAifOk;
    }

    const std::vector<std::string> options = splitString(armnnOptions, ';');
    std::vector<std::string> keys, values;

    for (const auto& option: options) {
        auto key_value = splitString(option, ':');
        if (key_value.size() != 2) {
            std::cout << "option string is wrong!!" << std::endl;
            return kAifError;
        }
        keys.emplace_back(key_value[0]);
        values.emplace_back(key_value[1]);
    }

    for (auto i=0; i < options.size(); i++) {
	std::cout << "key:" << keys[i] << ", value:" << values[i] << std::endl;
        if (keys[i] == std::string("backends")) {
            std::vector<armnn::BackendId> backends;
            auto backendsStr = splitString(values[i], ',');
            for (const auto& backend : backendsStr) {
                backends.push_back(backend);
            }
            m_delegateOptions.SetBackends(backends);
        }
        if (keys[i] == std::string("logging-severity")) {
            m_delegateOptions.SetLoggingSeverity(values[i]);
        }
        // TODO: other options....
    }

    return kAifOk;
}

ArmNNDelegatePtr ArmNNDelegate::createArmNNDelegate()
{
    return ArmNNDelegatePtr(armnnDelegate::TfLiteArmnnDelegateCreate(m_delegateOptions),
                            armnnDelegate::TfLiteArmnnDelegateDelete);
}

} // end of namespace aif
