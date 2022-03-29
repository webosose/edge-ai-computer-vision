#include <aif/sample/Sample.h>
#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/log/Logger.h>

namespace aif {

Sample::Sample(const std::string& configPath, const std::string& defaultConfig)
    : m_config(configPath, defaultConfig)
{
}

Sample::~Sample()
{
}

void Sample::init()
{
    m_model = m_config.getOption("model");
    m_param = m_config.getOptionObject("param");

    Logi("model: ", m_model);
    Logi("param: ", m_param);

    AIVision::init();
    m_detector = aif::DetectorFactory::get().getDetector(m_model, m_param);
}

ImageSample::ImageSample(
        const std::string& configPath,
        const std::string& defaultConfig,
        bool useReadImage)
    : Sample(configPath, defaultConfig)
    , m_useReadImage(useReadImage)
{
}

ImageSample::~ImageSample()
{
}

const cv::Mat& ImageSample::readImage()
{
    m_image = cv::imread(m_config.getOption("ImagePath"));
    return m_image;
}

void ImageSample::run()
{
    std::shared_ptr<Descriptor> descriptor = aif::DetectorFactory::get().getDescriptor(m_model);
    if (m_useReadImage) {
        m_detector->detect(readImage(), descriptor);
    } else {
        m_detector->detectFromImage(m_config.getOption("ImagePath"), descriptor);
    }

    onResult(descriptor);
}

}   // end of namespace aif
