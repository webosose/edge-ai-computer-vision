/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/DetectorOperation.h>
#include <aif/pipe/DetectorOperationRegistration.h>
#include <aif/base/DetectorFactory.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

DetectorOperation::DetectorOperation(const std::string& id)
: NodeOperation(id)
{
}

DetectorOperation::~DetectorOperation()
{
}

bool DetectorOperation::init(const std::shared_ptr<NodeOperationConfig>& config)
{
    if (!NodeOperation::init(config)) {
        return false;
    }

    std::shared_ptr<DetectorOperationConfig> doConfig =
        std::dynamic_pointer_cast<DetectorOperationConfig>(m_config);

    m_model = doConfig->getModel();
    m_param = doConfig->getParam();

    Logi(m_id, ": init detector operation");
    Logi(m_id, ": model - ", m_model);
    Logi(m_id, ": param - ", m_param);

    m_detector = DetectorFactory::get().getDetector(m_model, m_param);
    return (m_detector != nullptr);
}

bool DetectorOperation::runImpl(const std::shared_ptr<NodeInput>& input)
{
    if (!m_detector) {
        Loge(m_id, ": detector is null");
        return false;
    }

    std::shared_ptr<PipeDescriptor> pipeDescriptor = input->getDescriptor();
    if (!pipeDescriptor) {
        Loge(m_id, ": node descriptor is null");
        return false;
    }

    const cv::Mat& image = pipeDescriptor->getImage();
    std::shared_ptr<Descriptor> descriptor =
        DetectorFactory::get().getDescriptor(m_model);

    t_aif_status res = m_detector->detect(image, descriptor);
    if (res != kAifOk) {
        Loge(m_id, ": failed to detect ", m_model);
        return false;
    }

    pipeDescriptor->addDetectorOperationResult(m_id, m_model, descriptor);
    return true;
}

} // end of namespace aif
