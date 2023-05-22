/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/Pipe.h>
#include <aif/pipe/PipeDescriptorFactory.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Renderer.h>
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>

using aif::Stopwatch;

namespace aif {

Pipe::Pipe()
{
}

Pipe::~Pipe()
{
}

bool Pipe::build(const std::string& config)
{
    m_config = std::make_shared<PipeConfig>();
    if (!m_config->parse(config)) {
        Loge("failed to build pipe (invalid config): ", config);
        return false;
    }
    m_name = m_config->getName();
    Logi(m_name, ": build pipe ");
    Logi("config: ", config);

    for (int i = 0; i < m_config->getNodeSize(); i++) {
        auto nodeConfig = m_config->getNode(i);
        auto pipeNode = std::make_shared<PipeNode>();
        if (!pipeNode) {
            Loge(m_name, ": failed to create pipe node");
            return false;
        }
        if (!pipeNode->build(nodeConfig)) {
            Loge(m_name, ": failed to build pipe node");
            return false;
        }
        if (!addPipeNode(pipeNode)) {
            Loge(m_name, ": failed to add pipe node into pipe");
            return false;
        }
    }
    return true;
}

bool Pipe::detect(const cv::Mat& image)
{
    if (m_nodes.empty()) {
        Loge(m_name, ": empty pipe");
        return false;
    }

    Stopwatch sw;
    sw.start();
    m_descriptor = PipeDescriptorFactory::get().create(m_config->getDescriptorId());
    m_descriptor->setImage(image);

    m_descriptor->setStartTimestamp(sw.getTimestamp());

    std::shared_ptr<PipeNode> prevNode;
    for (auto& node : m_nodes) {
        if (!prevNode) {
            node->getInput()->setDescriptor(m_descriptor);
        } else {
            prevNode->moveDescriptor(node);
        }

        if (!node->run()) {
            return false;
        }
        prevNode = node;
    }
    Logw(m_name, ": pipe detect time: ", sw.getMs(), "ms");

    /* for debug!! */
    //drawPipeResults();

    sw.stop();
    return true;
}

bool Pipe::detectFromFile(const std::string& imagePath)
{
    if (m_nodes.empty()) {
        Loge(m_name, ": empty pipe");
        return false;
    }

    cv::Mat image;
    if (getCvImageFrom(imagePath, image) != kAifOk) {
        Loge("failed to read image file: ", imagePath);
        return false;
    }

    return detect(image);
}

bool Pipe::detectFromBase64(const std::string& base64Image)
{
    if (m_nodes.empty()) {
        Loge(m_name, ": empty pipe");
        return false;
    }

    cv::Mat image;
    if (getCvImageFromBase64(base64Image, image) != kAifOk) {
        Loge("failed to read base64: ", base64Image);
        return false;
    }

    return detect(image);
}

const std::string& Pipe::getName() const
{
    if (!m_config) {
        Loge("pipe is not ready (build pipe before getName)");
    }
    return m_name;
}

const std::shared_ptr<PipeDescriptor>& Pipe::getDescriptor() const
{
    return m_descriptor;
}

bool Pipe::addPipeNode(std::shared_ptr<PipeNode>& node)
{
    if (!m_nodes.empty()) {
        if (!m_nodes.back()->verifyConnectNext(node)) {
            Loge(m_name, ": failed to verifyConnectNext ",
                    m_nodes.back()->getId(), " -! ", node->getId());
            return false;
        }
    }
    m_nodes.emplace_back(node);
    return true;
}

void Pipe::drawPipeResults()
{
    Stopwatch sw;
    std::string outputPath = "/usr/share/aif/capturePipe_";
    outputPath += std::to_string(sw.getTimestamp());
    outputPath += ".jpg";

    Logi("result outputPath: ", outputPath);

    auto fd = std::dynamic_pointer_cast<FitTvPoseDescriptor>(m_descriptor);
    if (fd == nullptr) {
        Loge(__func__, "failed to convert Descriptor to FitTvPoseDescriptor");
        return;
    }

    cv::Mat result = fd->getImage();
    for (auto& keyPoints : fd->getKeyPoints()) {
        result = Renderer::drawPose2d(result, keyPoints);
    }

    result = Renderer::drawRects(result, fd->getCropRects(), cv::Scalar(255, 0, 0), 1);
    result = Renderer::drawBoxes(result, fd->getBboxes(), cv::Scalar(0, 0, 255), 2);

    cv::imwrite(outputPath, result);
}

} // end of namespace aif
