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
, mCropExtension(1.25)
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
    if (fdescriptor == nullptr) {
        Loge(__func__, "failed to convert Descriptor to FitTvPoseDescriptor");
        return false;
    }


    const cv::Mat& image = descriptor->getImage();

#if defined(AFFINE_TRANS)
    Logd("AFFINE_TRANS!!!");
    const std::vector<BBox>& boxes = fdescriptor->getBboxes();
    for (auto& box : boxes) {
        auto fixedBbox = fixBbox(input, box);
        Scale scale;
        computeCropsData(fixedBbox, scale.x, scale.y);
        fdescriptor->addCropData(scale); // pass scales
        fdescriptor->addCropBox(fixedBbox); // pass fixedBox
        fdescriptor->addCropImage(image); /* add original image */
    }
#else
    auto originalImg = input->getDescriptor()->getImage();
    auto originSize = input->getDescriptor()->getImage().size();

    const std::vector<BBox>& boxes = fdescriptor->getBboxes();
    for (auto& box : boxes) {
        auto fixedBbox = fixBbox(input, box);
        fdescriptor->addCropBox(fixedBbox); // pass fixedBox

        auto crop = getCropRect(originalImg, originSize, fixedBbox);
        fdescriptor->addCropImage(crop); /* add original image */
    }

    /*std::vector<cv::Mat> rects;
    fdescriptor->addCropRects(rects);

    fdescriptor->addBridgeOperationResult(
                 m_id,
                 m_config->getType(),
                 "person crop result");*/
#endif
    return true;
}

void FitTvPersonCropOperation::computeCropsData(const BBox& bbox, float& scaleX, float& scaleY)
{
    scaleX = bbox.width / 200.0f;
    scaleY = bbox.height / 200.0f;
}

// onnx imlementation new
cv::Mat FitTvPersonCropOperation::getCropRect(cv::Mat& originalImg, cv::Size& originSize, BBox& fixedBbox) const
{
    int modelInputWidth = 192;
    int modelInputHeight = 256;

    BBox scaledBbox = BBox(originSize.width, originSize.height);
    scaledBbox.addTlhw( fixedBbox.xmin, fixedBbox.ymin, fixedBbox.width, fixedBbox.height );

    int targetXmin = static_cast<int>(scaledBbox.xmin);
    int targetYmin = static_cast<int>(scaledBbox.ymin);
    int targetXmax = static_cast<int>(scaledBbox.xmax);
    int targetYmax = static_cast<int>(scaledBbox.ymax);
    int targetWidth = targetXmax - targetXmin;
    int targetHeight = targetYmax - targetYmin;

    cv::Mat fullCropImg = cv::Mat::zeros(targetHeight, targetWidth, originalImg.type());

    if (targetXmax > originSize.width) {
        targetWidth = targetWidth - (targetXmax - originSize.width);
    }
    if (targetYmax > originSize.height) {
        targetHeight = targetHeight - (targetYmax - originSize.height);
    }

    cv::Rect safeCropArea(0, 0, targetWidth, targetHeight);
    cv::Rect targetCropArea(targetXmin, targetYmin, targetWidth, targetHeight);

    cv::Mat targetCropImg = fullCropImg(safeCropArea);
    originalImg(targetCropArea).copyTo(targetCropImg);

    cv::Mat crop;
    cv::resize(fullCropImg, crop, cv::Size(modelInputWidth, modelInputHeight));

    return crop;
}


// onnx imlementation
std::vector<cv::Rect> FitTvPersonCropOperation::getCropRects_deprecated(const std::shared_ptr<NodeInput>& input) const
{
    std::vector<cv::Rect> rects;

    std::shared_ptr<FitTvPoseDescriptor> descriptor =
        std::dynamic_pointer_cast<FitTvPoseDescriptor>(input->getDescriptor());
    if (descriptor == nullptr) {
        Loge(__func__, "failed to convert Descriptor to FitTvPoseDescriptor");
        return rects;
    }

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
    int modelInputWidth = 192;
    int modelInputHeight = 256;

    auto originSize = input->getDescriptor()->getImage().size(); // image size

    float xmin = std::max(bbox.xmin, 0.0f);
    float ymin = std::max(bbox.ymin, 0.0f);
    float xmax = std::min(originSize.width - 1.0f, xmin + std::max(0.0f, bbox.width - 1.0f));
    float ymax = std::min(originSize.height - 1.0f, ymin + std::max(0.0f, bbox.height - 1.0f));
    float width = xmax - xmin + 1.0f;
    float height = ymax - ymin + 1.0f;

    auto aspectRatio = static_cast<float> (modelInputWidth) / static_cast<float> (modelInputHeight);

    if ((width / height) > aspectRatio)
    {
        height = (width / aspectRatio) * 1.25f;
        width = width * 1.25f;
    }
    else if ((width / height) < aspectRatio)
    {
        width = (height * aspectRatio) * 1.25f;
        height = height * 1.25f;
    }

    float targetXmin = std::max(0.0f, ((xmin + xmax) / 2.0f) - ((width / 2.0f)));
    float targetYmin = std::max(0.0f, ((ymin + ymax) / 2.0f) - ((height / 2.0f)));

    BBox targetBox(originSize.width, originSize.height);
    targetBox.addTlhw(targetXmin, targetYmin, width, height);
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
