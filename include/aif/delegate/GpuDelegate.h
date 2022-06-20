/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_GPUDELEGATE_H
#define AIF_GUPDELEGATE_H

#include <aif/base/Delegate.h>
#include <aif/base/DelegateFactory.h>
#include <aif/base/DelegateFactoryRegistrations.h>
#include <tensorflow/lite/delegates/gpu/delegate.h>

// For example json for gpu delegate option:
// {
//      "name": "gpu_delegate",
//      "option": {
//          "backends": ["cl"],
//          "inference_priorities": ["MIN_LATENCY", "MIN_MEMORY_USEAGE", "MAX_PRECISION"],
//          "enable_quntized_inference" : true
//      }
//  }
//
// Ordered priorities provide better control over desired semantics,
// where priority(n) is more important than priority(n+1), therefore,
// each time inference engine needs to make a decision, it uses
// ordered priorities to do so.
// For example:
//   MAX_PRECISION at priority1 would not allow to decrease precision,
//   but moving it to priority2 or priority3 would result in F16 calculation.
//
// Priority is defined in TfLiteGpuInferencePriority.
// AUTO priority can only be used when higher priorities are fully specified.
// For example:
//   VALID:   priority1 = MIN_LATENCY, priority2 = AUTO, priority3 = AUTO
//   VALID:   priority1 = MIN_LATENCY, priority2 = MAX_PRECISION,
//            priority3 = AUTO
//   INVALID: priority1 = AUTO, priority2 = MIN_LATENCY, priority3 = AUTO
//   INVALID: priority1 = MIN_LATENCY, priority2 = AUTO,
//            priority3 = MAX_PRECISION
// Invalid priorities will result in error.

namespace aif {

class GpuDelegate : public Delegate {
private:
    GpuDelegate(const std::string& option);

public:
    template<typename T> friend class DelegateFactoryRegistration;
    virtual ~GpuDelegate();

    TfLiteDelegatePtr getTfLiteDelegate() const override;
    const TfLiteGpuDelegateOptionsV2& getDelegateOptions() const;

private:
    void setupOptions();
    TfLiteGpuDelegateOptionsV2 m_delegateOptions;
};

DelegateFactoryRegistration<GpuDelegate> gpu_delegate("gpu_delegate");

} // end of namespace aif

#endif // AIF_GPUDELEGATE_H
