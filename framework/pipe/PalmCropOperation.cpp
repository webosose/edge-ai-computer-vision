/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/PalmCropOperation.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

PalmCropOperation::PalmCropOperation(const std::string& id)
: CropOperation(id)
{
}

PalmCropOperation::~PalmCropOperation()
{
}

cv::Rect PalmCropOperation::getCropRect(const std::shared_ptr<NodeInput>& input) const
{
    cv::Rect cropRect;
    std::shared_ptr<BridgeOperationConfig> config = std::dynamic_pointer_cast<BridgeOperationConfig>(m_config);
    if (config == nullptr) {
        Loge(__func__, "failed to convert NodeOperationConfig to BridgeOperationConfig");
        return cropRect;
    }

    const std::string& targetId = config->getTargetId();

    std::string result = input->getDescriptor()->getResult(targetId);
    if (result.empty()) {
        Loge(m_id, ": failed to get operation result from ", targetId);
        return cropRect;
    }
    rj::Document doc;
    doc.Parse(result.c_str());

    const rj::Value& target = doc.GetObject();
    if (!target.HasMember("palms") || target["palms"].Size() == 0) {
        Loge(m_id, ": failed to get palm element from ", result);
        return cropRect;
    }

    const rj::Value& palm =  target["palms"][0];
    if (!palm.HasMember("region")) {
        Loge(m_id, ": failed to get region from ", jsonObjectToString(palm));
        return cropRect;
    }

    const auto& region = palm["region"].GetArray();
    if (region.Size() < 4) {
        Loge(m_id, ": region is invalid", jsonObjectToString(palm));
        return cropRect;
    }
    cv::Size imgSize = input->getDescriptor()->getImage().size();
    cropRect.x = imgSize.width * region[0].GetFloat();
    cropRect.y = imgSize.height * region[1].GetFloat();
    cropRect.width = imgSize.width * region[2].GetFloat();
    cropRect.height = imgSize.height * region[3].GetFloat();

    int cx = cropRect.x + (cropRect.width/2);
    int cy = cropRect.y;

    cropRect.x = cx - (cropRect.width * 1.5f);
    cropRect.y = cy - (cropRect.height * 1.5f);
    cropRect.width = cropRect.width * 3;
    cropRect.height = cropRect.height* 3;

    if (cropRect.x < 0) cropRect.x = 0;
    if (cropRect.y < 0) cropRect.y = 0;
    if (cropRect.x + cropRect.width > imgSize.width)
        cropRect.width = imgSize.width - cropRect.x;
    if (cropRect.y + cropRect.height > imgSize.height)
        cropRect.height = imgSize.height- cropRect.y;

    Logi(m_id, ": rect ", cropRect);
    return cropRect;
}

} // end of namespace aif
