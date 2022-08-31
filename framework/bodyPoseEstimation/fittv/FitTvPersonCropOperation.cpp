/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/fittv/FitTvPersonCropOperation.h>
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

FitTvPersonCropOperation::FitTvPersonCropOperation(const std::string& id)
: BridgeOperation(id)
{
}

FitTvPersonCropOperation::~FitTvPersonCropOperation()
{
}

bool FitTvPersonCropOperation::runImpl(const std::shared_ptr<NodeInput>& input)
{
    auto& descriptor = input->getDescriptor();
    if (!descriptor) {
        Loge(m_id, ": failed to run operaiton (descriptor is null)");
        return false;
    }
    std::shared_ptr<FitTvPoseDescriptor> fdescriptor=
        std::dynamic_pointer_cast<FitTvPoseDescriptor>(input->getDescriptor());

    const cv::Mat& image = descriptor->getImage();
    auto rects = getCropRects(input);
    for (auto& rect : rects) {
        fdescriptor->addCropImage(image(rect));
    }

    fdescriptor->addCropRects(rects);
    return true;
}

std::vector<cv::Rect> FitTvPersonCropOperation::getCropRects(const std::shared_ptr<NodeInput>& input) const
{
    std::vector<cv::Rect> rects;

    std::shared_ptr<FitTvPoseDescriptor> descriptor =
        std::dynamic_pointer_cast<FitTvPoseDescriptor>(input->getDescriptor());

    const std::vector<BBox>& boxes = descriptor->getBboxes();
    for (auto& box :  boxes) {
        cv::Rect cropRect = {
            static_cast<int>(box.xmin),
            static_cast<int>(box.ymin),
            static_cast<int>(box.width),
            static_cast<int>(box.height) };

        int cx = cropRect.x + (cropRect.width / 2);
        int cy = cropRect.y + (cropRect.height / 2);

        cropRect.width = cropRect.width * 1.2f;
        cropRect.height = cropRect.height * 1.2f;

        cropRect.x = std::max(0, cx - (cropRect.width / 2));
        cropRect.y = std::max(0, cy - (cropRect.height / 2));

        auto originSize = input->getDescriptor()->getImage().size();
        if (originSize.width < cropRect.x + cropRect.width) {
            cropRect.width = originSize.width - cropRect.x;
        }
        if (originSize.height < cropRect.y + cropRect.height) {
            cropRect.height = originSize.height - cropRect.y;
        }
        rects.push_back(cropRect);
        Logi("crop rect: ", cropRect);
    }

    return rects;
}


} // end of namespace aif
