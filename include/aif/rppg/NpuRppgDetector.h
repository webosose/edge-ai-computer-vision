/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_RPPG_DETECTOR_H
#define AIF_NPU_RPPG_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/rppg/RppgDescriptor.h>
#include <aif/rppg/RppgDetector.h>

namespace aif {

class NpuRppgDetector : public RppgDetector {
  private:
    NpuRppgDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~NpuRppgDetector();

  protected:
    t_aif_status compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;
  private:
    float m_inputQuatScaleIn;
    float m_outputQuatScaleIn;
    float m_zeropointIn;
};

DetectorFactoryRegistration<NpuRppgDetector, RppgDescriptor>
    rppg_npu("rppg_npu");

} // end of namespace aif

#endif // AIF_NPU_RPPG_DETECTOR_H
