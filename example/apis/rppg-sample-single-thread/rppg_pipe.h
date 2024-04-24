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

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <queue>
#include <string>
#include <thread>
#include <rapidjson/document.h>
#include <fstream>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace aif
{
namespace rj = rapidjson;

using OnRppgResult = std::function<void(int id, const std::string &json)>;

typedef struct
{
    std::string dataPipeId;
    std::string dataPipeConfig;
    std::string inferencePipeId;
    std::string inferencePipeConfig;
    uint32_t dataAggregationSize;
    std::chrono::milliseconds inferenceInterval;
    OnRppgResult onResultFunc;
} RppgConfig;

typedef struct
{
    int id;
    double frameTime;
    double meshR;
    double meshG;
    double meshB;
} MeshData;

template <typename DATA>
class DataQueue
{
public:
    DataQueue() : m_isRunning(true){};
    virtual ~DataQueue(){};

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::queue<DATA> empty;
        m_queue.swap(empty);
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    void setAggregationSize(size_t num)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_aggregationSize = num;
    }

    std::pair<int, cv::Mat> getMatData()
    {
        std::unique_lock<std::mutex> ulock(m_mutex);
        m_cv.wait(ulock, [this] { return (m_queue.size() >= m_aggregationSize || !m_isRunning); });

        int id = -1;
        if (!m_isRunning)
        {
            return std::make_pair(id, cv::Mat());
        }
        std::vector<std::vector<double>> temp;
        /*
        // Get GT RGBI-------------------------------------------------------------------------------------------------
        int curr_id = (m_queue.back().id - 120) / 15 + 1;
        rj::Document doc;
        // Read with gt rgbi
        std::string gt_rgbi  = "./rgbi.json";
        std::ifstream ifs(gt_rgbi);
        if (!ifs.is_open()) {
            std::cout << "failed to read file : " << gt_rgbi << std::endl;
        }
        std::string pyRe;
        std::string line_py;
        while (getline(ifs, line_py)){
            pyRe.append("\n");
            pyRe.append(line_py);
        }

        doc.Parse(pyRe.c_str());
        std::vector<MeshData> input_rbgi;
        for (auto& values : doc.GetObject())
        {
            if(std::stoi(values.name.GetString()) == curr_id)
            {
                int count =0;
                MeshData meshd;
                for (auto& nums : values.value.GetArray())
                {
                    if (count >= 4) count = 0;
                    if (count == 0) meshd.meshR = nums.GetDouble();
                    else if(count == 1) meshd.meshG = nums.GetDouble();
                    else if(count == 2) meshd.meshB = nums.GetDouble();
                    else {
                        meshd.frameTime = nums.GetDouble();
                        input_rbgi.push_back(meshd);
                    }
                    count++;
                }
            }
        }

        // for(int i=0; i < input_rbgi.size(); i++){
        //     std::cout << input_rbgi[i].frameTime <<", "<< input_rbgi[i].meshR <<", "<< input_rbgi[i].meshG <<", "<< input_rbgi[i].meshB <<std::endl;
        // }
        for (size_t i = 0; i < m_queue.size(); i++)
        {
            std::vector<double> meshData;
            auto data = std::move(m_queue.front());
            auto data2 = std::move(input_rbgi[i]);
            data2.id = data.id;
            m_queue.pop();

            id = data.id;
            meshData.push_back(input_rbgi[i].frameTime);
            meshData.push_back(input_rbgi[i].meshR);
            meshData.push_back(input_rbgi[i].meshG);
            meshData.push_back(input_rbgi[i].meshB);
            temp.push_back(meshData);

            m_queue.push(std::move(data2));
        }
        // -------------------------------------------------------------------------------------------------
        */

        // /*
        for (size_t i = 0; i < m_queue.size(); i++)
        {
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
        // */

        cv::Mat mat(temp.size(), 4, CV_64F);
        for (size_t i = 0; i < temp.size(); i++)
        {
            for (size_t j = 0; j < 4; j++)
                mat.at<double>(i, j) = temp[i][j];
        }

        return std::make_pair<int, cv::Mat>(std::move(id), std::move(mat));
    }

    bool pop(DATA &data)
    {
        std::unique_lock<std::mutex> ulock(m_mutex);
        m_cv.wait(ulock, [this] { return (!m_queue.empty() || !m_isRunning); });

        if (!m_isRunning)
        {
            return false;
        }

        if (!m_queue.empty())
        {
            data = m_queue.front();
            m_queue.pop();
            return true;
        }
        return false;
    }

    void push(const DATA &data)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_queue.size() == m_aggregationSize)
            {
                m_queue.pop();
            }

            m_queue.push(data);
        }
        m_cv.notify_one();
    }

    void notifyAll() { m_cv.notify_all(); }

    bool isRunning() const { return m_isRunning; }
    void setStop()
    {
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

class RppgPipe
{
public:
    RppgPipe();
    ~RppgPipe();

    bool isStarted() const { return m_isStarted; }
    bool startup(const RppgConfig &config);
    bool shutdown();
    bool detect(int id, double frameTimeInterval, const cv::Mat &image);
    bool inference();

private:
    bool m_isStarted;
    RppgConfig m_config;
    DataQueue<MeshData> m_queue;
    std::thread m_thread;
    std::mutex m_callbackMutex;
};

} // namespace aif

#endif // AIF_RPPG_PIPE_H
