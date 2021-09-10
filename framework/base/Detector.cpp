#include <aif/base/Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

using aif::Stopwatch;

namespace aif {

Detector::Detector(
    const std::string& modelPath,
    const std::shared_ptr<DetectorParam>& param)
    : m_model(nullptr)
    , m_interpreter(nullptr)
    , m_modelPath(modelPath)
    , m_param(param)
{
}

Detector::~Detector()
{
}

t_aif_status Detector::init()
{
    std::stringstream errlog;
    try {
        Stopwatch sw;
        // compile model
        sw.start();
        if (compileModel() != kAifOk) {
            errlog.clear();
            errlog << "tflite model compile failed: " << m_modelPath;
            throw std::runtime_error(errlog.str());
        }
        TRACE("compileModel(): ", sw.getMs(), "ms");
        sw.stop();
        return preProcessing();
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status Detector::detectFromImage(
    const std::string& imagePath, std::shared_ptr<Descriptor>& descriptor)
{
    try {
        Stopwatch sw;
        cv::Mat img;
        // prepare input tensors
        sw.start();
        if (aif::getCvImageFrom(imagePath, img) != kAifOk) {
            throw std::runtime_error(std::string("can't get opencv image: ")
                + imagePath);
        }
        TRACE("aif::getCvImageFrom(): ", sw.getMs(), "ms");
        sw.stop();

        return detect(img, descriptor);
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status Detector::detectFromBase64(
    const std::string& base64image, std::shared_ptr<Descriptor>& descriptor)
{
    try {
        Stopwatch sw;
        cv::Mat img;
        // prepare input tensors
        sw.start();
        if (aif::getCvImageFromBase64(base64image, img) != kAifOk) {
            throw std::runtime_error("can't get opencv image from base64image!");
        }
        TRACE("aif::getCvImageFromBase64(): ", sw.getMs(), "ms");
        sw.stop();

        return detect(img, descriptor);
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status Detector::detect(
    const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    try {
        Stopwatch sw;
        // prepare input tensors
        sw.start();
        if (fillInputTensor(img) != kAifOk) {
            throw std::runtime_error("fill input tensor failed!!");
        }
        TRACE("\tfillInputTensor(): ", sw.getMs(), "ms");
        sw.stop();

        // start to inference
        sw.start();
        TfLiteStatus res = m_interpreter->Invoke();
        if (res != kTfLiteOk) {
            throw std::runtime_error("tflite interpreter invoke failed!!");
        }
        TRACE("\tm_interpreter->Invoke(): ", sw.getMs(), "ms");
        sw.stop();
        return postProcessing(img, descriptor);

    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

} // end of namespace aif