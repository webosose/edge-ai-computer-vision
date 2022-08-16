/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/delegate/NpuDelegate.h>
#include <aif/tools/Utils.h>
#include <rapidjson/document.h>

#include <fstream>
namespace rj = rapidjson;

namespace aif {

NpuDelegate::NpuDelegate(const std::string& option)
    : Delegate("NpuDelegate", option)
{
}

NpuDelegate::~NpuDelegate()
{
}

TfLiteDelegatePtr NpuDelegate::getTfLiteDelegate() const
{
    return TfLiteDelegatePtr(
            webos::npu::tflite::TfLiteNpuDelegateCreate(m_delegateOptions),
            webos::npu::tflite::TfLiteNpuDelegateDelete);
}

const webos::npu::tflite::NpuDelegateOptions& NpuDelegate::getDelegateOptions() const
{
    return m_delegateOptions;
}

} // end of namespace aif
