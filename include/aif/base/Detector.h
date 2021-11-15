#ifndef AIF_DETECTOR_H
#define AIF_DETECTOR_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>
#include <aif/base/Descriptor.h>

#include <tensorflow/lite/model.h>
#include <tensorflow/lite/interpreter.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

namespace aif {

class Detector
{
public:
    Detector(const std::string& modelPath, const std::shared_ptr<DetectorParam>& param);

    virtual ~Detector();
    virtual t_aif_status init();
    virtual t_aif_status detectFromImage(
        const std::string& imagePath, std::shared_ptr<Descriptor>& descriptor);
    virtual t_aif_status detectFromBase64(
        const std::string& base64image, std::shared_ptr<Descriptor>& descriptor);
    virtual t_aif_status detect(
        const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor);
    virtual t_aif_status setOptions(const std::string& options) { return kAifOk; };

    std::string getModelPath() const { return m_modelPath; }
    t_aif_modelinfo getModelInfo() const { return m_modelInfo; }
    std::shared_ptr<DetectorParam> getParam() const { return m_param; }
    void setParam(const std::shared_ptr<DetectorParam>& param) { m_param = param; }

protected:
    virtual t_aif_status compileModel() = 0;
    virtual t_aif_status fillInputTensor(const cv::Mat& img) = 0;
    virtual t_aif_status preProcessing() = 0;
    virtual t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) = 0;

protected:
    std::unique_ptr<tflite::FlatBufferModel> m_model;
    std::unique_ptr<tflite::Interpreter> m_interpreter;
    std::string m_modelPath;
    std::shared_ptr<DetectorParam> m_param;
    t_aif_modelinfo m_modelInfo;
};

} // end of namespace aif

#endif // AIF_DETECTOR_H
