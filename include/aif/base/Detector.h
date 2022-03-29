#ifndef AIF_DETECTOR_H
#define AIF_DETECTOR_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>
#include <aif/base/Descriptor.h>
#include <aif/base/Delegate.h>

#include <tensorflow/lite/model.h>
#include <tensorflow/lite/interpreter.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

namespace aif {

class Detector
{
public:
    Detector(const std::string& modelName);
    virtual ~Detector();

    virtual t_aif_status init(const std::string& param = "");
    virtual t_aif_status detectFromImage(
        const std::string& imagePath, std::shared_ptr<Descriptor>& descriptor);
    virtual t_aif_status detectFromBase64(
        const std::string& base64image, std::shared_ptr<Descriptor>& descriptor);
    virtual t_aif_status detect(
        const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor);

    const std::string& getModelName() const { return m_modelName; }
    const t_aif_modelinfo& getModelInfo() const { return m_modelInfo; }
    std::shared_ptr<DetectorParam> getParam() const { return m_param; }
    size_t getNumDelegates() const { return m_delegates.size(); }

protected:
    virtual std::shared_ptr<DetectorParam> createParam() = 0;
    virtual t_aif_status fillInputTensor(const cv::Mat& img) = 0;
    virtual t_aif_status preProcessing() = 0;
    virtual t_aif_status postProcessing(
            const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) = 0;
    virtual t_aif_status compileModel() = 0;

    virtual t_aif_status compile();
    virtual t_aif_status compileDelegates();

protected:
    std::string m_modelName;
    t_aif_modelinfo m_modelInfo;
    std::unique_ptr<tflite::FlatBufferModel> m_model;
    std::unique_ptr<tflite::Interpreter> m_interpreter;
    std::shared_ptr<DetectorParam> m_param;
    std::vector<std::shared_ptr<Delegate>> m_delegates;

};

} // end of namespace aif

#endif // AIF_DETECTOR_H
