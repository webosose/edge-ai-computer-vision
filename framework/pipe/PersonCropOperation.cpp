/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/PersonCropOperation.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

PersonCropOperation::PersonCropOperation(const std::string& id)
: CropOperation(id)
{
}

PersonCropOperation::~PersonCropOperation()
{
}

cv::Rect PersonCropOperation::getCropRect(const std::shared_ptr<NodeInput>& input) const
{
    cv::Rect cropRect;
    std::shared_ptr<BridgeOperationConfig> config = std::dynamic_pointer_cast<BridgeOperationConfig>(m_config);
    const std::string& targetId = config->getTargetId();

    std::string result = input->getDescriptor()->getResult(targetId);
    if (result.empty()) {
        Loge(m_id, ": failed to get operation result from ", targetId);
        return cropRect;
    }
    Logi("crop rect id: ", targetId);
    Logi("crop rect : ", result);

    rj::Document doc;
    doc.Parse(result.c_str());
    const rj::Value& target = doc.GetObject();
    if (!target.HasMember("persons") || target["persons"].Size() == 0) {
        Loge(m_id, ": failed to get bbox element from ", result);
        return cropRect;
    }

    const rj::Value& person = target["persons"][0];
    if (!person.HasMember("bbox")) {
        Loge(m_id, ": failed to get bbox from ", jsonObjectToString(person));
        return cropRect;
    }

    const auto& bbox= person["bbox"].GetArray();
    if (bbox.Size() < 6) {
        Loge(m_id, ": bbox is invalid", jsonObjectToString(person));
        return cropRect;
    }
    cropRect = {
        (int)bbox[0].GetFloat(),
        (int)bbox[1].GetFloat(),
        (int)bbox[4].GetFloat(),
        (int)bbox[5].GetFloat()};

    int cx = cropRect.x + (cropRect.width / 2);
    int cy = cropRect.y + (cropRect.height / 2);

    cropRect.width = cropRect.width * 1.4f;
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

    Logi("Final crop rect: ", cropRect);
    return cropRect;
}

// onnx modem implmentation
//cv::Rect PersonCropOperation::getCropRect(const std::shared_ptr<NodeInput>& input) const
//{
//    cv::Rect cropRect;
//    std::shared_ptr<BridgeOperationConfig> config = std::dynamic_pointer_cast<BridgeOperationConfig>(m_config);
//    const std::string& targetId = config->getTargetId();
//
//    std::string result = input->getDescriptor()->getResult(targetId);
//    if (result.empty()) {
//        Loge(m_id, ": failed to get operation result from ", targetId);
//        return cropRect;
//    }
//
//    std::cout << result << std::endl;
//
//    rj::Document doc;
//    doc.Parse(result.c_str());
//
//    const rj::Value& target = doc.GetObject();
//    if (!target.HasMember("persons") || target["persons"].Size() == 0) {
//        Loge(m_id, ": failed to get bbox element from ", result);
//        return cropRect;
//    }
//
//    const rj::Value& person = target["persons"][0];
//    if (!person.HasMember("bbox")) {
//        Loge(m_id, ": failed to get bbox from ", jsonObjectToString(person));
//        return cropRect;
//    }
//
//    const auto& bbox= person["bbox"].GetArray();
//    if (bbox.Size() < 6) {
//        Loge(m_id, ": bbox is invalid", jsonObjectToString(person));
//        return cropRect;
//    }
//
//    std::cout << "###################" << std::endl;
//    std::cout << bbox[0].GetFloat() << " "
//                << bbox[1].GetFloat() << " "
//                << bbox[2].GetFloat() << " "
//                << bbox[3].GetFloat() << std::endl;
//
//    int cx = (bbox[0].GetFloat() + bbox[2].GetFloat())/2;
//    int cy = (bbox[1].GetFloat() + bbox[3].GetFloat())/2;
//
//    int width = bbox[4].GetFloat() * 1.4f;
//    int height = bbox[5].GetFloat();
//
//    BBox targetBox(width, height);
//    targetBox.addXyxy(
//            cx - (width/2),
//            cy - (height/2),
//            cx + (width/2),
//            cy + (height/2), false);
//
//    std::cout << targetBox << std::endl;
//    std::cout << "###################" << std::endl;
//    auto orgImgSize= input->getDescriptor()->getImage().size();
//    auto cropBox = fixBbox(targetBox, orgImgSize);
//
//    return cv::Rect(cropBox.xmin, cropBox.ymin, cropBox.width, cropBox.height);
//}
//
//BBox PersonCropOperation::fixBbox(const BBox& bbox, const cv::Size& orgImgSize) const
//{
//    cv::Size modelSize(192, 256);
//
//    float scalex = std::max(bbox.width, 10.0f);
//    float scaley = std::max(bbox.height, 10.0f);
//    float arrayExpand = 1.0f;
//    if (bbox.width / bbox.height < 0.66f) {
//        arrayExpand = 0.77f;
//    }
//    float scale = std::max( scalex, scaley ) * arrayExpand;
//    float ratioW = ( scale / bbox.width ) - 1.0f;
//    float ratioH = ( scale / bbox.height ) - 1.0f;
//
//    BBox squareBox(orgImgSize.width, orgImgSize.height);
//    squareBox.addXyxy( bbox.xmin - ( ( ratioW * bbox.width ) / 2.0f ),
//            bbox.ymin - ( ( ratioH * bbox.height ) / 2.0f ),
//            bbox.xmax + ( ( ratioW * bbox.width ) / 2.0f ),
//            bbox.ymax + ( ( ratioH * bbox.height ) / 2.0f ), false );
//
//    // expand bbox by scale increase
//    float ar = static_cast<float>( modelSize.width) / modelSize.height;
//    float boxAr = squareBox.width / squareBox.height;
//
//    BBox targetBox(orgImgSize.width, orgImgSize.height);
//
//    float dh = 0;
//    float dw = 0;
//    if ( boxAr > ar )
//    {
//        dh = ( ( 1 / ar ) * squareBox.width ) - squareBox.height;
//    }
//    else if ( boxAr < ar )
//    {
//        dw = ( ar * squareBox.height ) - squareBox.width;
//    }
//    targetBox.addXyxy( static_cast<int>( squareBox.xmin - ( dw / 2 ) ),
//            static_cast<int>( squareBox.ymin - ( dh / 2 ) ),
//            static_cast<int>( squareBox.xmax + ( dw / 2 ) ),
//            static_cast<int>( squareBox.ymax + ( dh / 2 ) ), true);
//
//
//    return targetBox;
//}

} // end of namespace aif
