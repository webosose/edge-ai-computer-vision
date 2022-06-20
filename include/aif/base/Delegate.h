/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_DELEGATE_H
#define AIF_DELEGATE_H

#include <aif/base/Types.h>
#include <string>
#include <memory>

#include <tensorflow/lite/c/common.h>

namespace aif {

using TfLiteDelegatePtr = std::unique_ptr<TfLiteDelegate, void (*)(TfLiteDelegate*)>;

class Delegate {
public:
    Delegate(const std::string& name, const std::string& option = "");
    virtual ~Delegate();

    const std::string& getName() const;

    virtual TfLiteDelegatePtr getTfLiteDelegate() const = 0;

protected:
    std::string m_name;
    std::string m_option;
};

} // end of namespace aif

#endif // AIF_DELEGATE_H_
