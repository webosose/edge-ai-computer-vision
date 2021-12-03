#include <aif/delegate/ArmNNDelegate.h>
#include <aif/tools/Utils.h>

#include <rapidjson/document.h>
namespace rj = rapidjson;

namespace aif {

ArmNNDelegate::ArmNNDelegate(const std::string& option)
    : Delegate("ArmNNDelegate", option)
    , m_delegateOptions(armnn::Compute::CpuAcc)
{
    m_delegateOptions.SetBackends({"CpuAcc"});
    m_delegateOptions.SetLoggingSeverity("info");

    parseOption();
}

ArmNNDelegate::~ArmNNDelegate()
{
}

void ArmNNDelegate::parseOption()
{
    if (m_option.empty()) {
        Logi("ArmnnOptions empty, Apply default options");
        return;
    }

    Logi("Armnn delegate option: ", m_option);
    rj::Document payload;
    payload.Parse(m_option.c_str());
    if (payload.HasMember("backends")) {
        std::vector<armnn::BackendId> backends;
        for (auto& backend : payload["backends"].GetArray()) {
            backends.push_back(backend.GetString());
            Logi("backend: ", backend.GetString());
        }
    }

    if (payload.HasMember("logging_severity")) {
        m_delegateOptions.SetLoggingSeverity(payload["logging_severity"].GetString());
        Logi("logging_serverity: ", payload["logging_severity"].GetString());
    }
}

TfLiteDelegatePtr ArmNNDelegate::getTfLiteDelegate() const
{
    return TfLiteDelegatePtr(
            armnnDelegate::TfLiteArmnnDelegateCreate(m_delegateOptions),
            armnnDelegate::TfLiteArmnnDelegateDelete);
}

} // end of namespace aif
