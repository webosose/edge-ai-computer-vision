/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FITTV_PERSON_CROP_OPERATION_H
#define AIF_FITTV_PERSON_CROP_OPERATION_H

#include <aif/pipe/BridgeOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>
#include <aif/bodyPoseEstimation/common.h>

namespace aif {

class FitTvPersonCropOperation : public BridgeOperation {
    private:
        FitTvPersonCropOperation(const std::string& id);

    public:
        virtual ~FitTvPersonCropOperation();
        template<typename T1, typename T2>
            friend class NodeOperationFactoryRegistration;

        bool runImpl(const std::shared_ptr<NodeInput>& input) override;

    private:
        void computeCropsData(const BBox& bbox, const float expand, float& scaleX, float& scaleY);
        cv::Mat getCropRect(cv::Mat& originalImg, cv::Size& originSize, BBox& fixedBbox) const;
        std::vector<cv::Rect> getCropRects_deprecated(const std::shared_ptr<NodeInput>& input) const;
        BBox fixBbox(const cv::Size &originSize, const BBox& bbox) const;

    private:
        float mCropExtension;
};

NodeOperationFactoryRegistration<FitTvPersonCropOperation, BridgeOperationConfig>
    fittv_person_crop("fittv_person_crop");

} // end of namespace aif

#endif // AIF_FITTV_PERSON_CROP_OPERATION_H
