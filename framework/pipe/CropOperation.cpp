/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/CropOperation.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

CropOperation::CropOperation(const std::string& id)
: BridgeOperation(id)
{
}

CropOperation::~CropOperation()
{
}

bool CropOperation::runImpl(const std::shared_ptr<NodeInput>& input)
{
    auto& descriptor = input->getDescriptor();
    if (!descriptor) {
        Loge(m_id, ": failed to run operaiton (descriptor is null)");
        return false;
    }

    const cv::Mat& image = descriptor->getImage();
    cv::Size imageSize = image.size();
    cv::Rect cropRect = getCropRect(input);
    if (cropRect.width == 0 || cropRect.height == 0 ||
        imageSize.width < cropRect.x + cropRect.width ||
        imageSize.height < cropRect.y + cropRect.height) {
        Loge(m_id, ": failed to crop image (crop size is wrong)");
        Loge("image size : ", imageSize);
        Loge("crop size : ", cropRect);
        return false;
    }

    if (m_id.rfind("rppg_face_crop", 0) == 0) {
        auto cropRect_image = image(cropRect);
        cv::Mat resizedImg;
        cv::resize(cropRect_image, resizedImg, cv::Size(), 2, 2, cv::INTER_LINEAR);
        descriptor->setImage(resizedImg);
        // imwrite("upScale_face.jpg", descriptor->getImage());
    }
    else {
        descriptor->setImage(image(cropRect));
    }

    rj::Document doc;
    doc.SetObject();
    rj::Document::AllocatorType& allocator = doc.GetAllocator();
    rj::Value cropRegion(rj::kArrayType);
    cropRegion.PushBack(cropRect.x, allocator);
    cropRegion.PushBack(cropRect.y, allocator);
    cropRegion.PushBack(cropRect.width, allocator);
    cropRegion.PushBack(cropRect.height, allocator);
    doc.GetObject().AddMember("region", cropRegion, allocator);
    descriptor->addBridgeOperationResult(
            m_id,
            m_config->getType(),
            jsonObjectToString(doc.GetObject()));

    return true;
}


} // end of namespace aif
