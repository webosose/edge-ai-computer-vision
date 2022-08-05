/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_DETECTOR_OPERATION_H
#define AIF_DETECTOR_OPERATION_H

#include <aif/pipe/NodeOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>
#include <aif/base/Detector.h>

#include <string>

namespace aif {

class DetectorOperation : public NodeOperation {
    private:
       DetectorOperation(const std::string& id);

    public:
       virtual ~DetectorOperation();

       template<typename T1, typename T2>
       friend class NodeOperationFactoryRegistration;

       bool init(const std::shared_ptr<NodeOperationConfig>& config) override;

    protected:
       bool runImpl(const std::shared_ptr<NodeInput>& input) override;

    private:
       std::shared_ptr<Detector> m_detector;
       std::string m_model;
       std::string m_param;

};

NodeOperationFactoryRegistration<DetectorOperation, DetectorOperationConfig>
    detector_operation("detector");

} // end of namespace aif

#endif // AIF_DETECTOR_OPERATION_H
