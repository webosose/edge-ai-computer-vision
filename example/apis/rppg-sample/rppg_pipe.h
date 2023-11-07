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

#ifndef AIF_RPPG_PIPE_H
#define AIF_RPPG_PIPE_H

#include <aif/facade/EdgeAIVision.h>

#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <opencv2/opencv.hpp>

namespace aif {

using OnRppgResult = std::function<void(int id, const std::string& json)>;

typedef struct {
    std::string dataPipeId;
    std::string dataPipeConfig;
    std::string inferencePipeId;
    std::string inferencePipeConfig;
    uint32_t dataAggregationSize;
    std::chrono::milliseconds inferenceInterval;
    OnRppgResult onResultFunc;
} RppgConfig;

typedef struct {
    int id;
    double frameTime;
    double meshR;
    double meshG;
    double meshB;
} MeshData;

template<typename DATA>
class DataQueue {
public:
    DataQueue() : m_isRunning(true) {};
    virtual ~DataQueue() {};

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    void setAggregationSize(size_t num) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_aggregationSize = num;
    }

    std::pair<int, cv::Mat> getMatData() {
        std::unique_lock<std::mutex> ulock(m_mutex);
        m_cv.wait(ulock, [this] {return (m_queue.size() >= m_aggregationSize || !m_isRunning); });

        int id = -1;
        if (!m_isRunning) { return std::make_pair(id, cv::Mat()); }
        std::vector<std::vector<double>> temp;
        for (size_t i = 0; i < m_queue.size(); i++) {
            std::vector<double> meshData;
            auto data = std::move(m_queue.front());
            m_queue.pop();

            id = data.id;
            meshData.push_back(data.frameTime);
            meshData.push_back(data.meshR);
            meshData.push_back(data.meshG);
            meshData.push_back(data.meshB);
            temp.push_back(meshData);

            m_queue.push(std::move(data));
        }

        cv::Mat mat(temp.size(), 4, CV_64F);
        for(auto i = 0; i < temp.size(); i++){
            for(auto j = 0; j < 4; j++)
                mat.at<double>(i, j) = temp[i][j];
        }

        return std::make_pair<int, cv::Mat>(std::move(id), std::move(mat));
    }

    bool pop(DATA& data) {
        std::unique_lock<std::mutex> ulock(m_mutex);
        m_cv.wait(ulock, [this] {return (!m_queue.empty() || !m_isRunning); });

        if (!m_isRunning) { return false; }

        if(!m_queue.empty()) {
            data = m_queue.front();
            m_queue.pop();
            return true;
        }
        return false;
    }

    void push(const DATA& data) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_queue.size() == m_aggregationSize) {
                m_queue.pop();
            }

            m_queue.push(data);
        }
        m_cv.notify_one();
    }

    void notifyAll() {
        m_cv.notify_all();
    }

    bool isRunning() const { return m_isRunning; }
    void setStop() {
        m_isRunning = false;
        notifyAll();
    }

private:
    std::queue<DATA> m_queue;
    std::condition_variable_any m_cv;
    mutable std::mutex m_mutex;
    size_t m_aggregationSize;
    bool m_isRunning;
};

class RppgInferenceThread {
public:
    RppgInferenceThread(RppgConfig& config, EdgeAIVision& ai, DataQueue<MeshData>& queue)
       : m_config(config), m_ai(ai), m_queue(queue) {}
    ~RppgInferenceThread() {}
    void operator()();

private:
    RppgConfig& m_config;
    EdgeAIVision& m_ai;
    DataQueue<MeshData>& m_queue;
};


class RppgPipe {
public:
    RppgPipe();
    ~RppgPipe();

    bool isStarted() const { return m_isStarted; }
    bool startup(const RppgConfig& config);
    bool shutdown();
    bool detect(int id, double frameTimeInterval, const cv::Mat& image);

private:
    bool m_isStarted;
    RppgConfig m_config;
    DataQueue<MeshData> m_queue;
    std::thread m_thread;
};

}

#endif // AIF_RPPG_PIPE_H
