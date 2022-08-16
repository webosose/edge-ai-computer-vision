/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PIPE_DESCRIPTOR_H
#define AIF_PIPE_DESCRIPTOR_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <aif/pipe/NodeType.h>
#include <aif/base/Descriptor.h>

#include <rapidjson/document.h>
#include <opencv2/core/core.hpp>

#include <string>
#include <memory>

namespace rj = rapidjson;

namespace aif {

class PipeDescriptor : public Descriptor {
    public:
        PipeDescriptor();
        virtual ~PipeDescriptor();

        void setImage(const cv::Mat& image);
        const NodeType& getType() const;
        const cv::Mat& getImage() const;

        std::string getResult() const;
        std::string getResult(const std::string& id) const;

        virtual bool addBridgeOperationResult(
                const std::string& id,
                const std::string& result);

        virtual bool addDetectorOperationResult(
                const std::string& id,
                const std::string& model,
                const std::shared_ptr<Descriptor>& descriptor);

    protected:
        NodeType m_type;
        cv::Mat m_image;
};

} // end of namespace aif

#endif // AIF_PIPE_DESCRIPTOR_H
