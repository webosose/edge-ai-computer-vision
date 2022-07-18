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

void MNameDetector::setModelInfo(TfLiteTensor* inputTensor)
{
    m_modelInfo.batchSize = inputTensor->dims->data[0];
    m_modelInfo.height = inputTensor->dims->data[1];
    m_modelInfo.width = inputTensor->dims->data[2];
    m_modelInfo.channels = inputTensor->dims->data[3];

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);
}

t_aif_status MNameDetector::fillInputTensor(const cv::Mat& img)/* override*/
{

    // TODO: implement to fill input tensor
    //    try {
    //        if (img.rows == 0 || img.cols == 0) {
    //            throw std::runtime_error("invalid opencv image!!");
    //        }
    //
    //        int height = m_modelInfo.height;
    //        int width = m_modelInfo.width;
    //        int channels = m_modelInfo.channels;
    //
    //        if (m_interpreter == nullptr) {
    //            throw std::runtime_error("cpu_model_name interpreter not initialized!!");
    //        }
    //
    //        t_aif_status res = aif::fillInputTensor<uint8_t, cv::Vec3b>(
    //                m_interpreter.get(),
    //                img,
    //                width,
    //                height,
    //                channels,
    //                true,
    //                aif::kAifNone
    //                );
    //        if (res != kAifOk) {
    //            throw std::runtime_error("fillInputTensor failed!!");
    //        }
    //
    //        return res;
    //    } catch(const std::exception& e) {
    //        Loge(__func__,"Error: ", e.what());
    //        return kAifError;
    //    } catch(...) {
    //        Loge(__func__,"Error: Unknown exception occured!!");
    //        return kAifError;
    //    }
    return kAifOk;
}

t_aif_status MNameDetector::preProcessing()
{
    return kAifOk;
}

t_aif_status MNameDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    const std::vector<int> &outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(outputs[0]);
    float* keyPoints = reinterpret_cast<float*>(output->data.data);

    std::shared_ptr<MNameDescriptor> mnameDescriptor = std::dynamic_pointer_cast<MNameDescriptor>(descriptor);

    // TODO: implement to add result into descriptor
    // mnameDescriptor->add()
    
    return kAifOk;
}

} // end of namespace aif
