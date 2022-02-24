#include <aif/bodypix/BodypixDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

BodypixDetector::BodypixDetector(const std::string& modelPath)
    : Detector(modelPath)
{
}

BodypixDetector::~BodypixDetector()
{
}

std::shared_ptr<DetectorParam> BodypixDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<BodypixParam>();
    return param;
}

t_aif_status BodypixDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv image!!");
        }

        int height = m_modelInfo.height;
        int width = m_modelInfo.width;
        int channels = m_modelInfo.channels;

        if (m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter not initialized!!");
        }

        t_aif_status res = aif::fillInputTensor<uint8_t, cv::Vec3b>(
                m_interpreter.get(),
                img,
                width,
                height,
                channels,
                true,
                aif::kAifNone
                );
        if (res != kAifOk) {
            throw std::runtime_error("fillInputTensor failed!!");
        }

        return res;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status BodypixDetector::preProcessing()
{
    return kAifOk;
}

t_aif_status BodypixDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    const std::vector<int> &t_outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(t_outputs[5]);
    uint8_t* data= reinterpret_cast<uint8_t*>(output->data.data);
    int height = output->dims->data[1];
    int width = output->dims->data[2];

    TRACE("size : ", height, " x ", width);

    std::shared_ptr<BodypixDescriptor> bodypixDescriptor = std::dynamic_pointer_cast<BodypixDescriptor>(descriptor);
    bodypixDescriptor->addMaskData(width, height, data);

    return kAifOk;
}

} // end of namespace aif
