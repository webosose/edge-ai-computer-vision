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
    descriptor->setImage(image(cropRect));

    rj::Document doc;
    doc.SetObject();
    rj::Document::AllocatorType& allocator = doc.GetAllocator();
    rj::Value cropRegion(rj::kArrayType);
    cropRegion.PushBack(cropRect.x, allocator);
    cropRegion.PushBack(cropRect.y, allocator);
    cropRegion.PushBack(cropRect.width, allocator);
    cropRegion.PushBack(cropRect.height, allocator);
    doc.GetObject().AddMember("region", cropRegion, allocator);
    descriptor->addOperationResult(m_id, jsonObjectToString(doc.GetObject()));

    return true;
}


} // end of namespace aif