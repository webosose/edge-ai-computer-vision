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

// onnx imlementation
std::vector<cv::Rect> FitTvPersonCropOperation::getCropRects(const std::shared_ptr<NodeInput>& input) const
{
    std::vector<cv::Rect> rects;

    std::shared_ptr<FitTvPoseDescriptor> descriptor =
        std::dynamic_pointer_cast<FitTvPoseDescriptor>(input->getDescriptor());

    const std::vector<BBox>& boxes = descriptor->getBboxes();
    for (auto& box :  boxes) {

        auto fixedBbox = fixBbox(input, box);
        cv::Rect cropRect = {
            static_cast<int>(fixedBbox.xmin),
            static_cast<int>(fixedBbox.ymin),
            static_cast<int>(fixedBbox.width),
            static_cast<int>(fixedBbox.height) };

        auto originSize = input->getDescriptor()->getImage().size();
        if (cropRect.x < 0) cropRect.x = 0;
        if (cropRect.x > originSize.width) cropRect.x = originSize.width;
        if (cropRect.y < 0) cropRect.y = 0;
        if (cropRect.y > originSize.height) cropRect.y = originSize.height;
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

BBox FitTvPersonCropOperation::fixBbox(const std::shared_ptr<NodeInput>& input, const BBox& bbox) const
{
    auto originSize = input->getDescriptor()->getImage().size();
    int modelInputWidth = 192;
    int modelInputHeight = 256;

    float scalex = std::max( bbox.width, 10.0f );
    float scaley = std::max( bbox.height, 10.0f );
    float arrayExpand = 1.0f;
    if ( bbox.width / bbox.height < 0.66 )
    {
        arrayExpand = 0.77;
    }
    float scale = std::max( scalex, scaley ) * arrayExpand;
    float ratioW = ( scale / bbox.width ) - 1.0f;
    float ratioH = ( scale / bbox.height ) - 1.0f;

    BBox squareBox(originSize.width, originSize.height);
    squareBox.addXyxy( bbox.xmin - ( ( ratioW * bbox.width ) / 2.0f ),
            bbox.ymin - ( ( ratioH * bbox.height ) / 2.0f ),
            bbox.xmax + ( ( ratioW * bbox.width ) / 2.0f ),
            bbox.ymax + ( ( ratioH * bbox.height ) / 2.0f ), false );

    // expand bbox by scale increase
    float ar = static_cast<float>( modelInputWidth ) / modelInputHeight;
    float boxAr = squareBox.width / squareBox.height;

    BBox targetBox(originSize.width, originSize.height);
    float dh = 0;
    float dw = 0;
    if ( boxAr > ar )
    {
        dh = ( ( 1 / ar ) * squareBox.width ) - squareBox.height;
    }
    else if ( boxAr < ar )
    {
        dw = ( ar * squareBox.height ) - squareBox.width;
    }
    targetBox.addXyxy( static_cast<int>( squareBox.xmin - ( dw / 2 ) ),
            static_cast<int>( squareBox.ymin - ( dh / 2 ) ),
            static_cast<int>( squareBox.xmax + ( dw / 2 ) ),
            static_cast<int>( squareBox.ymax + ( dh / 2 ) ), false );

    return targetBox;
}


// origin implementation
//std::vector<cv::Rect> FitTvPersonCropOperation::getCropRects(const std::shared_ptr<NodeInput>& input) const
//{
//    std::vector<cv::Rect> rects;
//
//    std::shared_ptr<FitTvPoseDescriptor> descriptor =
//        std::dynamic_pointer_cast<FitTvPoseDescriptor>(input->getDescriptor());
//
//    const std::vector<BBox>& boxes = descriptor->getBboxes();
//    for (auto& box :  boxes) {
//        cv::Rect cropRect = {
//            static_cast<int>(box.xmin),
//            static_cast<int>(box.ymin),
//            static_cast<int>(box.width),
//            static_cast<int>(box.height) };
//
//        int cx = cropRect.x + (cropRect.width / 2);
//        int cy = cropRect.y + (cropRect.height / 2);
//
//        cropRect.width = cropRect.width * 1.2f;
//        cropRect.height = cropRect.height * 1.2f;
//
//        cropRect.x = std::max(0, cx - (cropRect.width / 2));
//        cropRect.y = std::max(0, cy - (cropRect.height / 2));
//
//        auto originSize = input->getDescriptor()->getImage().size();
//        if (originSize.width < cropRect.x + cropRect.width) {
//            cropRect.width = originSize.width - cropRect.x;
//        }
//        if (originSize.height < cropRect.y + cropRect.height) {
//            cropRect.height = originSize.height - cropRect.y;
//        }
//        rects.push_back(cropRect);
//        Logi("crop rect: ", cropRect);
//    }
//
//    return rects;
//}


} // end of namespace aif
