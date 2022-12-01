/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PIPE_H
#define AIF_PIPE_H

#include <aif/pipe/PipeConfig.h>
#include <aif/pipe/PipeNode.h>
#include <vector>

namespace aif {

class Pipe {
    public:
        Pipe();
        virtual ~Pipe();

        bool build(const std::string& config);
        bool detect(const cv::Mat& image);
        bool detectFromFile(const std::string& imagePath);
        bool detectFromBase64(const std::string& base64Image);

        const std::string& getName() const;
        const std::shared_ptr<PipeDescriptor>& getDescriptor() const;

    protected:
        bool addPipeNode(std::shared_ptr<PipeNode>& node);
        void drawPipeResults();

    private:
        std::string m_name;
        std::shared_ptr<PipeConfig> m_config;
        std::shared_ptr<PipeDescriptor> m_descriptor;
        std::vector<std::shared_ptr<PipeNode>> m_nodes;

};

} // end of namespace aif

#endif // AIF_PIPE_H
