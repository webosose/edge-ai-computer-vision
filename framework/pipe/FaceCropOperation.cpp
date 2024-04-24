/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/FaceCropOperation.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

FaceCropOperation::FaceCropOperation(const std::string& id)
: CropOperation(id)
{
}

FaceCropOperation::~FaceCropOperation()
{
}

cv::Rect FaceCropOperation::getCropRect(const std::shared_ptr<NodeInput>& input) const
{
    cv::Rect cropRect;
    std::shared_ptr<BridgeOperationConfig> config = std::dynamic_pointer_cast<BridgeOperationConfig>(m_config);
    if (config == nullptr)
    {
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
    if (!target.HasMember("faces") || target["faces"].Size() == 0) {
        Loge(m_id, ": failed to get face element from ", result);
        return cropRect;
    }

    const rj::Value &face = target["faces"][0];
    if (!face.HasMember("region")) {
        Loge(m_id, ": failed to get region from ", jsonObjectToString(face));
        return cropRect;
    }

    const auto &region = face["region"].GetArray();
    if (region.Size() < 4) {
        Loge(m_id, ": region is invalid", jsonObjectToString(face));
        return cropRect;
    }
    cv::Size imgSize = input->getDescriptor()->getImage().size();
    const int faceX = imgSize.width * region[0].GetFloat();
    const int faceY = imgSize.height * region[1].GetFloat();
    const int faceW = imgSize.width * region[2].GetFloat();
    const int faceH = imgSize.height * region[3].GetFloat();

    // add 25% padding
    cropRect.x = faceX - faceW * 0.25 > 0 ? faceX - faceW * 0.25 : 0;
    cropRect.y = faceY - faceH * 0.25 > 0 ? faceY - faceH * 0.25 : 0;
    cropRect.width =  faceW * 1.5 < imgSize.width  ? faceW * 1.5 : imgSize.width;
    cropRect.height = faceH * 1.5 < imgSize.height ? faceH * 1.5 : imgSize.height;

    Logd(m_id, ": rect ", cropRect);
    return cropRect;
}

} // end of namespace aif
