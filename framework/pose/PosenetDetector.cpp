#include <aif/pose/PosenetDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

PosenetDetector::PosenetDetector(
    const std::string& modelPath,
    const std::shared_ptr<DetectorParam>& param)
    : Detector(modelPath, param)
{
}

PosenetDetector::~PosenetDetector()
{
}

t_aif_status PosenetDetector::fillInputTensor(const cv::Mat& img)/* override*/
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

t_aif_status PosenetDetector::preProcessing()
{
    return kAifOk;
}

t_aif_status PosenetDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    const std::vector<int> &t_outputs = m_interpreter->outputs();
    TfLiteTensor *output= m_interpreter->tensor(t_outputs[0]);
    TRACE("output bytes: ", output->bytes);
    TRACE("output type: ", output->type);
    TRACE("output dims 0: ", output->dims->data[0]);
    TRACE("output dims 1: ",  output->dims->data[1]);
    TRACE("output dims 2: ",  output->dims->data[2]);
    TRACE("output dims 3: ",  output->dims->data[3]);

    TfLiteTensor *output3 = m_interpreter->tensor(t_outputs[1]);
    float* pose_score = output3->data.f;

    TfLiteTensor *output4 = m_interpreter->tensor(t_outputs[3]);
    float* pose_count = output4->data.f;
    TRACE("poses count: ", *pose_count);

    std::shared_ptr<PosenetDescriptor> posenetDescriptor = std::dynamic_pointer_cast<PosenetDescriptor>(descriptor);
    float* keyPoints = reinterpret_cast<float*>(output->data.data);
    float scaleX = (float)img.size().width / (float)m_modelInfo.width;
    float scaleY = (float)img.size().height / (float)m_modelInfo.height;

    TRACE("posenet img size: ", img.size());
    TRACE("scale x: ", scaleX);
    TRACE("scale y: ", scaleY);

    int k = 0;
    for (int i = 0; i < *pose_count; i++ ) {
        std::vector<cv::Point> points;
        for (int j = 0; j < 17; j++ ) {
            float height = keyPoints[k] * scaleY;
            float width = keyPoints[k+1] * scaleX;
            points.push_back(cv::Point(width, height));
            k = k + 2;
        }
        posenetDescriptor->addKeyPoints(points);
    }
    m_prev_poses = posenetDescriptor->makeBodyParts(m_prev_poses);
    

    return kAifOk;
}



} // end of namespace aif
