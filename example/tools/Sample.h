/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_SAMPLE_H
#define AIF_SAMPLE_H

#include <aif/base/Types.h>
#include <aif/base/Detector.h>
#include <aif/base/Descriptor.h>
#include <aif/tools/ConfigReader.h>

#include <opencv2/opencv.hpp>
#include <string>
#include <memory>

namespace aif {

class Sample
{
public:
    Sample(const std::string& configPath, const std::string& defaultConfig = "");
    virtual ~Sample();

    virtual void init();
    virtual void run() = 0;
    virtual void onResult(std::shared_ptr<Descriptor>& descriptor) = 0;

protected:
    ConfigReader m_config;
    std::shared_ptr<Detector> m_detector;
    std::string m_model;
    std::string m_param;
    std::string m_configPath;
};

class ImageSample : public Sample
{
public:
    ImageSample(
            const std::string& configPath,
            const std::string& defaultConfig = "",
            bool useReadImage = false);
    virtual ~ImageSample();
    virtual void run() override;

protected:
    virtual const cv::Mat& readImage();

    bool m_useReadImage;
    cv::Mat m_image;
};

}   // end of namespace aif

#endif // end of namespace aif
