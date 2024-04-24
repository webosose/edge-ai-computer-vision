/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bgSegment/BgSegmentDetector.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

BgSegmentDetector::BgSegmentDetector(const std::string& modelPath)
    : Detector(modelPath)
{
}

BgSegmentDetector::~BgSegmentDetector()
{
}

std::shared_ptr<DetectorParam> BgSegmentDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<BgSegmentParam>();
    return param;
}
#if 0
t_aif_status BgSegmentDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv image!!");
        }

        int height = m_modelInfo.height;
        int width = m_modelInfo.width;
        int channels = m_modelInfo.channels;

        if (m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter not initialized!!");
        }

        t_aif_status res = aif::fillInputTensor<uint8_t, cv::Vec3b>(
                m_interpreter.get(),
                img,
                width,
                height,
                channels,
                true,
                aif::kAifNone
                );
        if (res != kAifOk) {
            throw std::runtime_error("fillInputTensor failed!!");
        }

        return res;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status BgSegmentDetector::preProcessing()
{
    return kAifOk;
}

void BgSegmentDetector::printOutput() {
    const std::vector<int>& outputs = m_interpreter->outputs();
    for (int i = 0; i < outputs.size(); i++) {
        TfLiteTensor *output= m_interpreter->tensor(outputs[i]);
        TRACE(i, ":output bytes: ", output->bytes);
        TRACE(i, ":output type: ", output->type);
        TRACE(i, ":output dims 0: ", output->dims->data[0]);
        TRACE(i, ":output dims 1: ",  output->dims->data[1]);
        TRACE(i, ":output dims 2: ",  output->dims->data[2]);
        TRACE(i, ":output dims 3: ",  output->dims->data[3]);
    }
}

t_aif_status BgSegmentDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    printOutput();
    const std::vector<int> &t_outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(t_outputs[0]);
    int64_t* data= reinterpret_cast<int64_t*>(output->data.data);
    int height = output->dims->data[1];
    int width = output->dims->data[2];

    TRACE("size : ", height, " x ", width);

    std::shared_ptr<BgSegmentDescriptor> bgSegmentDescriptor = std::dynamic_pointer_cast<BgSegmentDescriptor>(descriptor);

    // CID9333382, CID9333365
    if (bgSegmentDescriptor != nullptr) {
        bgSegmentDescriptor->addMaskData(width, height, data);
    }

    return kAifOk;
}
#endif
} // end of namespace aif
