/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PERSON_CROP_OPERATION_H
#define AIF_PERSON_CROP_OPERATION_H

#include <aif/pipe/CropOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>
#include <aif/bodyPoseEstimation/common.h>

namespace aif {

class PersonCropOperation : public CropOperation {
    private:
        PersonCropOperation(const std::string& id);

    protected:
        cv::Rect getCropRect(const std::shared_ptr<NodeInput>& input) const override;
        BBox fixBbox(const BBox& bbox, const cv::Size& orgImgSize) const;

    public:
        virtual ~PersonCropOperation();
        template<typename T1, typename T2>
            friend class NodeOperationFactoryRegistration;

};

NodeOperationFactoryRegistration<PersonCropOperation, BridgeOperationConfig>
    person_crop_operation("person_crop");

} // end of namespace aif

#endif // AIF_PERSON_CROP_OPERATION_H
