#ifndef AIF_ARMNN_POSENET_DETECTOR_H
#define AIF_ARMNN_POSENET_DETECTOR_H

#include <aif/pose/PosenetDetector.h>
#include <armnn/delegate/armnn_delegate.hpp>

namespace aif {

using ArmNNDelegatePtr = std::unique_ptr<TfLiteDelegate, decltype(&armnnDelegate::TfLiteArmnnDelegateDelete)>;
class ArmNNPosenetDetector : public PosenetDetector
{
public:
    ArmNNPosenetDetector(
        const std::string& model_path,
        const std::string& options);
    virtual ~ArmNNPosenetDetector();

protected:
    t_aif_status compileModel() override;

private:
    std::vector<std::string> splitString(const std::string& str,
                                         const char delim);
    bool parseDelegateOptions(armnnDelegate::DelegateOptions& delegateOptions);

    ArmNNDelegatePtr m_delegate;
    std::string m_Options;
};


} // end of namespace aif

#endif // AIF_ARMNN_POSENET_DETECTOR_H
