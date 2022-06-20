/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_DELEGATE_FACTORY_REGISTRATION_H
#define AIF_DELEGATE_FACTORY_REGISTRATION_H

#include <aif/base/Types.h>
#include <aif/base/Delegate.h>
#include <aif/base/DelegateFactory.h>

#include <string>

namespace aif {

template <typename T>
class DelegateFactoryRegistration
{
public:
    DelegateFactoryRegistration(const std::string& id) {
        DelegateFactory::get().registerGenerator(
                id,
                [](const std::string& option) {
                    std::shared_ptr<Delegate> delegate(new T(option));
                    return delegate;
                }
        );
    }
};

} // end of namespace aif

#endif  // AIF_DELEGATE_FACTORY_REGISTRATION_H
