/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NNAPIDELEGATE_H
#define AIF_NNAPIDELEGATE_H

#include <aif/base/Delegate.h>
#include <aif/base/DelegateFactory.h>
#include <aif/base/DelegateFactoryRegistrations.h>
#include <tensorflow/lite/delegates/nnapi/nnapi_delegate.h>

namespace aif {
class NnapiDelegate : public Delegate {
private:
    NnapiDelegate(const std::string& option);

public:
    template<typename T> friend class DelegateFactoryRegistration;
    virtual ~NnapiDelegate();

    TfLiteDelegatePtr getTfLiteDelegate() const override;
    const tflite::StatefulNnApiDelegate::Options& getDelegateOptions() const;

private:
    void setupOptions();
    tflite::StatefulNnApiDelegate::Options m_delegateOptions;

};

DelegateFactoryRegistration<NnapiDelegate> nnapi_delegate("nnapi_delegate");

} // end of namespace aif

#endif // AIF_NNAPIDELEGATE_H

