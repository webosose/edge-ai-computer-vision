/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <VideoTester.h>
#include <aif/log/Logger.h>

namespace aif {
VideoTester::VideoTester()
: m_frameCount(0)
{
}

VideoTester::~VideoTester()
{
}

bool VideoTester::open(const std::string& inputPath,
                       const std::string& outputPath)
{
    m_capture.open(inputPath, cv::CAP_ANY);
    if (!m_capture.isOpened()) {
        Loge("failed to open file: ", inputPath);
        return false;
    }

    int ex = static_cast<int>(m_capture.get(cv::CAP_PROP_FOURCC));
    char EXT[] = {(char)(ex & 0XFF), (char)((ex & 0XFF00) >> 8),
                  (char)((ex & 0XFF0000) >> 16),
                  (char)((ex & 0XFF000000) >> 24), 0};
    m_frameCount = m_capture.get(cv::CAP_PROP_FRAME_COUNT);
    m_fps = m_capture.get(cv::CAP_PROP_FPS);
    cv::Size inputSize = cv::Size(m_capture.get(cv::CAP_PROP_FRAME_WIDTH),
                                  m_capture.get(cv::CAP_PROP_FRAME_HEIGHT));

    Logi("VideoCapture Info: ");
    Logi("frame count: ", m_frameCount);
    Logi("input size:", inputSize);
    Logi("fps : ", m_fps);
    Logi("codec : ", EXT);


    if (!outputPath.empty()) {
        m_writer.open(outputPath, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), m_fps,
                inputSize, true);
        if (!m_writer.isOpened()) {
            Loge("failed to open file: ", outputPath);
            m_writer.release();
            return false;
        }
    }

    return true;
}

bool VideoTester::close()
{
    if (m_capture.isOpened()) {
        m_capture.release();
    }
    if (m_writer.isOpened()) {
        m_writer.release();
    }
    return true;
}

bool VideoTester::test(const std::string& inputPath,
                       const std::string& outputPath,
                       OnFrameCallback onFrame)
{
    if (!open(inputPath, outputPath)) {
        return false;
    }
    cv::Mat src, dst;

    for (int i = 0; i < m_frameCount; i++) {
        m_capture >> src;
        if (src.empty()) {
            Logi("Video End");
            break;
        }
        dst = onFrame(src);
        if (m_writer.isOpened()) {
            m_writer << dst;
        } else {
            Loge("Video Writer is not opened");
        }

        Logi("VideoTester Progress: ", i + 1, " / ", m_frameCount);
    }
    if (!close()) {
        return false;
    }

    return true;
}

bool VideoTester::testWithFrameCount(
        const std::string& inputPath,
        const std::string& outputPath,
        OnFrameCallback onFrame,
        size_t frameCount)
{
    if (!open(inputPath, outputPath)) {
        return false;
    }

    cv::Mat src, dst;
    size_t count = (frameCount == 0) ? m_frameCount : std::min(frameCount, m_frameCount);
    for (int i = 0; i < count; i++) {
        m_capture >> src;
        if (src.empty()) {
            Logi("Video End");
            break;
        }
        dst = onFrame(src);
        if (m_writer.isOpened()) {
            m_writer << dst;
        } else {
            Loge("Video Writer is not opened");
        }


        Logi("VideoTester Progress: ", i + 1, " / ", count);
    }
    if (!close()) {
        return false;
    }

    return true;
}

} // namespace aif
