/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NODE_DESCRIPTOR_H
#define AIF_NODE_DESCRIPTOR_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <aif/pipe/NodeType.h>
#include <rapidjson/document.h>
#include <opencv2/core/core.hpp>

#include <string>
#include <memory>

namespace rj = rapidjson;

namespace aif {

class NodeDescriptor {
    public:
        NodeDescriptor();
        virtual ~NodeDescriptor();

        void setImage(const cv::Mat& image);
        void addOperationResult(
                const std::string& name,
                const std::string& result);

        const NodeType& getType() const;
        const cv::Mat& getImage() const;

        std::string getResult() const;

    private:
        NodeType m_type;
        cv::Mat m_image;
        rj::Document m_result;
};

} // end of namespace aif

#endif // AIF_NODE_DESCRIPTOR_H
