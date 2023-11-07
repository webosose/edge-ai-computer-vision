// @@@LICENSE
//
// Copyright (C) 2023, LG Electronics, All Right Reserved.
//
// No part of this source code may be communicated, distributed, reproduced
// or transmitted in any form or by any means, electronic or mechanical or
// otherwise, for any purpose, without the prior written permission of
// LG Electronics.
//
// LICENSE@@@


// solution manager plugin logger
//#define LOG_CONTEXT "solution.rppg"
//#define LOG_TAG "RppgPipe"
//#include <camera_log.h>

// edgeai vision sample logger
#define PLOGI printf
#define PLOGE printf
#define PLOGW printf
#define PLOGD printf

#include "rppg_pipe.h"
#include <rapidjson/document.h>

namespace aif {
namespace rj = rapidjson;

////////////////////////////////////////////////////////////////////////////////////
// RppgInferenceThread Implementation

void RppgInferenceThread::operator()() {
    if (!m_ai.pipeCreate(m_config.inferencePipeId, m_config.inferencePipeConfig)) {
        PLOGE("failed to create rppg inference pipe: %s", m_config.inferencePipeConfig.c_str());
        return;
    }

    bool isFirstInference = true;
    while (m_queue.isRunning()) {
        std::string output;
        auto start = std::chrono::high_resolution_clock::now();
        auto data = m_queue.getMatData();

        if (isFirstInference) {
            isFirstInference = false;
            start = std::chrono::high_resolution_clock::now();
        }

        if (!m_ai.pipeDetect(m_config.inferencePipeId, data.second, output)) {
            PLOGE("failed to detect: %s", m_config.inferencePipeId.c_str());
        } else {
            PLOGD("detect: %s", m_config.inferencePipeId.c_str());
            if (m_config.onResultFunc) {
                m_config.onResultFunc(data.first, output);
            } else {
                PLOGE("Error: empty result func");
            }
        }
        auto inferenceDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
        if (m_config.inferenceInterval > inferenceDuration) {
            auto sleepDuration = m_config.inferenceInterval - inferenceDuration;
            PLOGD("Sleep for : %llu ms", sleepDuration.count());
            std::this_thread::sleep_for(m_config.inferenceInterval - inferenceDuration);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
// RppgPipe Implementation
RppgPipe::RppgPipe() : m_isStarted(false)
{
}

RppgPipe::~RppgPipe()
{
}

bool RppgPipe::startup(const RppgConfig& config)
{
    if (m_isStarted) {
        PLOGE("failed to startup rppg pipe (already started)");
        return false;
    }
    m_config = config;
    m_queue.setAggregationSize(m_config.dataAggregationSize);

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    if (!ai.startup()) {
        PLOGE("failed to start edgeai vision");
        return false;
    }
    if (!ai.pipeCreate(config.dataPipeId, config.dataPipeConfig)) {
        PLOGE("failed to create rppg data aggregation pipe: %s", config.dataPipeConfig.c_str());
        return false;
    }
    m_thread = std::thread(RppgInferenceThread(m_config, ai, m_queue));

    m_isStarted = true;
    return true;
}

bool RppgPipe::shutdown()
{
    if (!m_isStarted) {
        PLOGE("failed to shutdown rppg pipe (already shutdown)");
        return false;
    }

    m_queue.setStop();
    if (m_thread.joinable()) {
        m_thread.join();
    }

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    if (!ai.shutdown()) {
        PLOGE("failed to shutdown edgeai vision");
        return false;
    }

    m_isStarted = false;
    return true;
}

bool RppgPipe::detect(int id, double frameTimeInterval, const cv::Mat& image)
{
    std::string output;

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    if (!ai.pipeDetect(m_config.dataPipeId, image, output)) {
        PLOGE("failed to detect data aggregation pipe");
    }

    rj::Document d;
    d.Parse(output.c_str());

    std::string nodeId = "extract_face_mesh_rgb";
    for (auto& result : d["results"].GetArray()) {
        if (result.HasMember(nodeId.c_str())) {
            MeshData data;
            data.id = id;
            data.frameTime = frameTimeInterval;
            data.meshR = result[nodeId.c_str()]["meshData"][0].GetFloat();
            data.meshG = result[nodeId.c_str()]["meshData"][1].GetFloat();
            data.meshB = result[nodeId.c_str()]["meshData"][2].GetFloat();
            m_queue.push(data);
            break;
        }
    }

    PLOGD("pipe_rggp_data detected : %u", m_queue.size());
    return true;
}

}
