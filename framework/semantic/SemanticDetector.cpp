#include <aif/semantic/SemanticDetector.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

SemanticDetector::SemanticDetector(const std::string& modelPath)
    : Detector(modelPath)
{
}

SemanticDetector::~SemanticDetector()
{
}

std::shared_ptr<DetectorParam> SemanticDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<SemanticParam>();
    return param;
}

t_aif_status SemanticDetector::fillInputTensor(const cv::Mat& img)/* override*/
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

t_aif_status SemanticDetector::preProcessing()
{
    return kAifOk;
}

void SemanticDetector::printOutput() {
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

t_aif_status SemanticDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    printOutput();
    const std::vector<int> &t_outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(t_outputs[0]);
    int64_t* data= reinterpret_cast<int64_t*>(output->data.data);
    int height = output->dims->data[1];
    int width = output->dims->data[2];

    TRACE("size : ", height, " x ", width);

    std::shared_ptr<SemanticDescriptor> semanticDescriptor = std::dynamic_pointer_cast<SemanticDescriptor>(descriptor);
    semanticDescriptor->addMaskData(width, height, data);

    return kAifOk;
}

} // end of namespace aif
