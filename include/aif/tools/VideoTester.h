/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_VIDEO_TESTER_H
#define AIF_VIDEO_TESTER_H

#include <functional>
#include <rapidjson/document.h>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

namespace aif {

class VideoTester
{
  public:
    VideoTester();
    ~VideoTester();

    using OnFrameCallback = std::function<cv::Mat(const cv::Mat&)>;
    bool test(const std::string& inputPath, const std::string& outputPath,
              OnFrameCallback onFrame);

  private:
    bool open(const std::string& inputPath, const std::string& outputPath);
    bool close();

  private:
    cv::VideoCapture m_capture;
    cv::VideoWriter m_writer;
    int m_frameCount;
};

} // end of namespace aif

#endif // AIF_VIDEO_TESTER_H
