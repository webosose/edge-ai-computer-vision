#ifndef AIF_ARMNNDELEGATE_H
#define AIF_ARMNNDELEGATE_H

#include <armnn/delegate/armnn_delegate.hpp>
#include <armnn/delegate/DelegateOptions.hpp>
#include <aif/base/Delegate.h>
#include <aif/base/DelegateFactory.h>
#include <aif/base/DelegateFactoryRegistrations.h>

namespace aif {

class ArmNNDelegate : public Delegate {
private:
    ArmNNDelegate(const std::string& option);

public:
    template<typename T> friend class DelegateFactoryRegistration;
    virtual ~ArmNNDelegate();

    TfLiteDelegatePtr getTfLiteDelegate() const override;

private:
    void parseOption();
    armnnDelegate::DelegateOptions m_delegateOptions;
};

DelegateFactoryRegistration<ArmNNDelegate> armnn_delegate("armnn_delegate");

} // end of namespace aif

#endif // AIF_ARMNNDELEGATE_H
