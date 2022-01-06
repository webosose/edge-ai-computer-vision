#include <aif/mname/MNameDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

MNameDetector::MNameDetector(const std::string& modelPath)
    : Detector(modelPath)
{
}

MNameDetector::~MNameDetector()
{
}

std::shared_ptr<DetectorParam> MNameDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<MNameParam>();
    return param;
}

t_aif_status MNameDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv image!!");
        }

        int height = m_modelInfo.height;
        int width = m_modelInfo.width;
        int channels = m_modelInfo.channels;

        if (m_interpreter == nullptr) {
            throw std::runtime_error("cpu_model_name interpreter not initialized!!");
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

t_aif_status MNameDetector::preProcessing()
{
    return kAifOk;
}

void MNameDetector::printOutput() {
    const std::vector<int>& outputs = m_interpreter->outputs();
    for (int i = 0; i < outputs.size(); i++) {
        TfLiteTensor *output= m_interpreter->tensor(outputs[i]);
        TRACE(i, ":output bytes: ", output->bytes);
        TRACE(i, ":output type: ", output->type);
        TRACE(i, ":output dims 0: ", output->dims->data[0]);
        TRACE(i, ":output dims 1: ",  output->dims->data[1]);
        TRACE(i, ":output dims 2: ",  output->dims->data[2]);
        TRACE(i, ":output dims 3: ",  output->dims->data[3]);
    }
}

t_aif_status MNameDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    const std::vector<int> &outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(outputs[0]);
    float* keyPoints = reinterpret_cast<float*>(output->data.data);

    std::shared_ptr<MNameDescriptor> mnameDescriptor = std::dynamic_pointer_cast<MNameDescriptor>(descriptor);
    float scaleX = (float)img.size().width / (float)m_modelInfo.width;
    float scaleY = (float)img.size().height / (float)m_modelInfo.height;

    TRACE("mname img size: ", img.size());
    TRACE("mname model size: ", m_modelInfo.width, m_modelInfo.height);
    TRACE("scale x: ", scaleX);
    TRACE("scale y: ", scaleY);

    int k = 0;
    std::vector<cv::Point2f> points;
    std::vector<float> scores;
    for (int j = 0; j < 17; j++ ) {
        float height = keyPoints[k]; // * scaleY;
        float width = keyPoints[k+1];// * scaleX;
        points.push_back(cv::Point2f(width, height));
        scores.push_back(keyPoints[k+2]);
        k = k + 3;
    }
    mnameDescriptor->addKeyPoints(points, scores);
    return kAifOk;
}

} // end of namespace aif
