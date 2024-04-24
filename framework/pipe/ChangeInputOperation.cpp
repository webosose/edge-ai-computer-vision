/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/ChangeInputOperation.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

ChangeInputOperation::ChangeInputOperation(const std::string& id)
: BridgeOperation(id)
{
}

ChangeInputOperation::~ChangeInputOperation()
{
}

bool isRectContained(const cv::Rect& a, const cv::Rect& b) {
    int a_right = a.x + a.width;
    int a_bottom = a.y + a.height;

    int b_right = b.x + b.width;
    int b_bottom = b.y + b.height;

    return (a.x <= b.x && a.y <= b.y && a_right >= b_right && a_bottom >= b_bottom);
}

bool ChangeInputOperation::runImpl(const std::shared_ptr<NodeInput>& input)
{
    auto& descriptor = input->getDescriptor();
    if (!descriptor) {
        Loge(m_id, ": failed to run operaiton (descriptor is null)");
        return false;
    }

    std::shared_ptr<ChangeInputOperationConfig> config
        = std::dynamic_pointer_cast<ChangeInputOperationConfig>(m_config);

    cv::Rect origRect(0, 0, descriptor->getImage().size().width, descriptor->getImage().size().height);
    cv::Rect changeRect = config->getChangeRect();

    if (!isRectContained(origRect, changeRect)) {
        Loge(m_id, ": failed to change rect");
        Loge("orig rect : ", origRect.x, ", " , origRect.y, ", ",
                origRect.width, ", " , origRect.height);
        Loge("change rect : ", changeRect.x, ", " , changeRect.y, ", ",
                changeRect.width, ", " , changeRect.height);

        return false;
    }

    const cv::Mat& image = descriptor->getImage();
    descriptor->setImage(image(config->getChangeRect()));

    return true;
}

} // end of namespace aif
