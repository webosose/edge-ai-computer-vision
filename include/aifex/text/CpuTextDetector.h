/* * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_TEXT_DETECTOR_H
#define AIF_CPU_TEXT_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aifex/text/TextDescriptor.h>
#include <aifex/text/TextDetector.h>

namespace aif {

class CpuTextDetector320 : public TextDetector {
  private:
    CpuTextDetector320();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuTextDetector320();
};

DetectorFactoryRegistration<CpuTextDetector320, TextDescriptor>
    text_paddleocr_320("text_paddleocr_320");

class CpuTextDetector256 : public TextDetector {
  private:
    CpuTextDetector256();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuTextDetector256();
};

DetectorFactoryRegistration<CpuTextDetector256, TextDescriptor>
    text_paddleocr_256("text_paddleocr_256");


class CpuTextDetector320_V2 : public TextDetector {
  private:
    CpuTextDetector320_V2();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuTextDetector320_V2();
};

DetectorFactoryRegistration<CpuTextDetector320_V2, TextDescriptor>
    text_padlleocr_320_v2("text_paddleocr_320_v2");

class CpuTextDetector256_V2 : public TextDetector {
  private:
    CpuTextDetector256_V2();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuTextDetector256_V2();
};

DetectorFactoryRegistration<CpuTextDetector256_V2, TextDescriptor>
    text_paddleocr_256_v2("text_paddleocr_256_v2");

} // end of namespace aif

#endif // AIF_CPU_TEXT_DETECTOR_H
