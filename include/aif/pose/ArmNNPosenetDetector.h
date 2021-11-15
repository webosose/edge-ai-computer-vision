#ifndef AIF_ARMNN_POSENET_DETECTOR_H
#define AIF_ARMNN_POSENET_DETECTOR_H

#include <aif/pose/PosenetDetector.h>
#include <aif/pose/PosenetDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/delegate/ArmNNDelegate.h>

namespace aif {

class ArmNNPosenetDetector : public PosenetDetector
{
public:
    ArmNNPosenetDetector();
    virtual ~ArmNNPosenetDetector();

    t_aif_status setOptions(const std::string& options) override;
protected:
    t_aif_status compileModel() override;

private:
    std::unique_ptr<ArmNNDelegate> m_delegateProvider;
    ArmNNDelegatePtr m_delegatePtr;
};

DetectorFactoryRegistration<ArmNNPosenetDetector, PosenetDescriptor>
posenet_mobilenet_armnn("posenet_mobilenet_armnn");

} // end of namespace aif

#endif // AIF_ARMNN_POSENET_DETECTOR_H
