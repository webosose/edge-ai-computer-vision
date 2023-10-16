#include "RppgPipe.h"
#include <rapidjson/document.h>

namespace aif {
namespace rj = rapidjson;

////////////////////////////////////////////////////////////////////////////////////
// RppgInferenceThread Implementation

void RppgInferenceThread::operator()() {
    if (!m_ai.pipeCreate(m_config.inferencePipeId, m_config.inferencePipeConfig)) {
        std::cout << "failed to create rppg inference pipe: " << m_config.inferencePipeConfig << std::endl;
        return;
    }

    while (m_queue.isRunning()) {
        std::string output;
        if (m_queue.size() >= m_config.dataAggregationSize) {
            auto start = std::chrono::high_resolution_clock::now();
            auto data = m_queue.getMatData();
            if (!m_ai.pipeDetect(m_config.inferencePipeId, data.second, output)) {
                std::cout << "failed to detect: " << m_config.inferencePipeId << std::endl;
            } else {
                std::cout << "detect: " << m_config.inferencePipeId << std::endl;
                if (m_config.onResultFunc) {
                    m_config.onResultFunc(data.first, output);
                } else {
                    std::cout << "Error: empty result func" << std::endl;
                }
            }
            auto inferenceDuration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
            if (m_config.inferenceInterval > inferenceDuration) {
                auto sleepDuration = m_config.inferenceInterval - inferenceDuration;
                std::cout << "Sleep for : " << sleepDuration.count() << " ms" << std::endl;
                std::this_thread::sleep_for(m_config.inferenceInterval - inferenceDuration);
            }
        } else {
            std::cout << "Sleep for : 100 ms" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        std::cout << "failed to startup rppg pipe (already started)" << std::endl;
        return false;
    }
    m_config = config;
    m_queue.setAggregationSize(m_config.dataAggregationSize);

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    if (!ai.startup()) {
        std::cout << "failed to start edgeai vision" << std::endl;
        return false;
    }
    if (!ai.pipeCreate(config.dataPipeId, config.dataPipeConfig)) {
        std::cout << "failed to create rppg data aggregation pipe: " << config.dataPipeConfig << std::endl;
        return false;
    }
    m_thread = std::thread(RppgInferenceThread(m_config, ai, m_queue));

    m_isStarted = true;
    return true;
}

bool RppgPipe::shutdown()
{
    if (!m_isStarted) {
        std::cout << "failed to shutdown rppg pipe (already shutdown)" << std::endl;
        return false;
    }

    m_queue.setStop();
    if (m_thread.joinable()) {
        m_thread.join();
    }

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    if (!ai.shutdown()) {
        std::cout << "failed to shutdown edgeai vision" << std::endl;
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
        std::cout << "failed to detect data aggregation pipe" << std::endl;
    }

    //std::cout << output << std::endl;
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
            std::cout << "push data: " << data.id << " " << data.frameTime << " "
                << data.meshR << " " << data.meshG << " " << data.meshB << std::endl;

            m_queue.push(data);
            break;
        }
    }

    std::cout << "pipe_rggp_data detected : " << m_queue.size()  << std::endl;
    return true;
}

}
