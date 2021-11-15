#ifndef AIF_ARMNNDELEGATE_H
#define AIF_ARMNNDELEGATE_H

#include <armnn/delegate/armnn_delegate.hpp>
#include <armnn/delegate/DelegateOptions.hpp>
#include <aif/base/Types.h>
#include <string>
#include <vector>

namespace aif {

using ArmNNDelegatePtr = std::unique_ptr<TfLiteDelegate, decltype(&armnnDelegate::TfLiteArmnnDelegateDelete)>;
class ArmNNDelegate {
public:
    ArmNNDelegate();
    virtual ~ArmNNDelegate();

    t_aif_status setArmNNDelegateOptions(const std::string& armnnOptions);
    ArmNNDelegatePtr createArmNNDelegate();

private:
    std::vector<std::string> splitString(const std::string& str,
                                         const char delim);

private:
    armnnDelegate::DelegateOptions m_delegateOptions;
};

} // end of namespace aif

#endif // AIF_ARMNNDELEGATE_H
