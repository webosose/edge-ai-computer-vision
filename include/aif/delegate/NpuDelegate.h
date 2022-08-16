/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPUDELEGATE_H
#define AIF_NPUDELEGATE_H

#include <aif/base/Delegate.h>
#include <aif/base/DelegateFactory.h>
#include <aif/base/DelegateFactoryRegistrations.h>
#include <aif/npu//npu_delegate.h>

namespace aif {

class NpuDelegate : public Delegate {
private:
    NpuDelegate(const std::string& option);

public:
    template<typename T> friend class DelegateFactoryRegistration;
    virtual ~NpuDelegate();

    TfLiteDelegatePtr getTfLiteDelegate() const override;
    const webos::npu::tflite::NpuDelegateOptions& getDelegateOptions() const;

private:
    webos::npu::tflite::NpuDelegateOptions m_delegateOptions;

};

DelegateFactoryRegistration<NpuDelegate> npu_delegate("npu_delegate");

} // end of namespace aif

#endif // AIF_NPUDELEGATE_H

