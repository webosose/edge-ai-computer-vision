#ifndef AIF_CPU_POSELANDMARK_DETECTOR_H
#define AIF_CPU_POSELANDMARK_DETECTOR_H

#include <aif/poseLandmark/PoseLandmarkDetector.h>
#include <aif/poseLandmark/PoseLandmarkDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/delegate/XnnpackDelegate.h>

namespace aif {

class CpuPoseLandmarkDetector : public PoseLandmarkDetector
{
protected:
    CpuPoseLandmarkDetector(const std::string& modelPath);

public:
    virtual ~CpuPoseLandmarkDetector();

protected:
    t_aif_status compileModel() override;
};


class LiteCpuPoseLandmarkDetector : public CpuPoseLandmarkDetector
{
private:
    LiteCpuPoseLandmarkDetector();

public: 
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~LiteCpuPoseLandmarkDetector() {}
};
DetectorFactoryRegistration<LiteCpuPoseLandmarkDetector, PoseLandmarkDescriptor>
poseLandmark_lite_cpu("poselandmark_lite_cpu");


class HeavyCpuPoseLandmarkDetector : public CpuPoseLandmarkDetector
{
private:
    HeavyCpuPoseLandmarkDetector();

public:    
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~HeavyCpuPoseLandmarkDetector() {}
};
DetectorFactoryRegistration<HeavyCpuPoseLandmarkDetector, PoseLandmarkDescriptor>
poseLandmark_heavy_cpu("poselandmark_heavy_cpu");


class FullCpuPoseLandmarkDetector : public CpuPoseLandmarkDetector
{
private:
    FullCpuPoseLandmarkDetector();

public:    
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~FullCpuPoseLandmarkDetector() {}
};
DetectorFactoryRegistration<FullCpuPoseLandmarkDetector, PoseLandmarkDescriptor>
poseLandmark_full_cpu("poselandmark_full_cpu");


} // end of namespace aif

#endif // AIF_CPU_POSELANDMARK_DETECTOR_H
