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
public:
    CpuPoseLandmarkDetector(
            const std::string& modelPath,
            const std::shared_ptr<DetectorParam>& param);
    virtual ~CpuPoseLandmarkDetector();

protected:
    t_aif_status setOptions(const std::string& options);
    t_aif_status compileModel() override;

private:
    std::unique_ptr<XnnpackDelegate> m_delegateProvider;
    bool m_useXnnpack;
    int m_numThreads;
};


class LiteCpuPoseLandmarkDetector : public CpuPoseLandmarkDetector
{
public:
    LiteCpuPoseLandmarkDetector();
    virtual ~LiteCpuPoseLandmarkDetector() {}
};
DetectorFactoryRegistration<LiteCpuPoseLandmarkDetector, PoseLandmarkDescriptor>
poseLandmark_lite_cpu("poselandmark_lite_cpu");


class HeavyCpuPoseLandmarkDetector : public CpuPoseLandmarkDetector
{
public:
    HeavyCpuPoseLandmarkDetector();
    virtual ~HeavyCpuPoseLandmarkDetector() {}
};
DetectorFactoryRegistration<HeavyCpuPoseLandmarkDetector, PoseLandmarkDescriptor>
poseLandmark_heavy_cpu("poselandmark_heavy_cpu");


class FullCpuPoseLandmarkDetector : public CpuPoseLandmarkDetector
{
public:
    FullCpuPoseLandmarkDetector();
    virtual ~FullCpuPoseLandmarkDetector() {}
};
DetectorFactoryRegistration<FullCpuPoseLandmarkDetector, PoseLandmarkDescriptor>
poseLandmark_full_cpu("poselandmark_full_cpu");


} // end of namespace aif

#endif // AIF_CPU_POSELANDMARK_DETECTOR_H
