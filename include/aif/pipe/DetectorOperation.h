/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_DETECTOR_OPERATION_H
#define AIF_DETECTOR_OPERATION_H

#include <aif/pipe/NodeOperation.h>
#include <aif/base/Detector.h>

#include <string>

namespace aif {

class DetectorOperation : public NodeOperation {
    protected:
       DetectorOperation(const std::string& id);

    public:
       virtual ~DetectorOperation();

       template<typename T1, typename T2>
       friend class NodeOperationFactoryRegistration;

       bool init(const std::shared_ptr<NodeOperationConfig>& config) override;
       bool update(const std::shared_ptr<NodeOperationConfig>& config) override;

    protected:
       bool runImpl(const std::shared_ptr<NodeInput>& input) override;

    protected:
       std::shared_ptr<Detector> m_detector;
       std::string m_model;
       std::string m_param;

};

} // end of namespace aif

#endif // AIF_DETECTOR_OPERATION_H
