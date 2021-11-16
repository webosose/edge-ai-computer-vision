#ifndef AIF_DETECTOR_FACTORY_H
#define AIF_DETECTOR_FACTORY_H

#include <aif/base/Types.h>
#include <aif/base/Detector.h>
#include <aif/base/Descriptor.h>

#include <string>

namespace aif {

using DetectorGenerator = std::shared_ptr<Detector>(*)();
using DescriptorGenerator = std::shared_ptr<Descriptor>(*)();

class DetectorFactory
{
public:
    static DetectorFactory& get() {
        static DetectorFactory instance;
        return instance;
    }
    t_aif_status registerGenerator(
            const std::string& id,
            const DetectorGenerator& detectorGenerator,
            const DescriptorGenerator& descriptorGenerator);
    std::shared_ptr<Detector> getDetector(const std::string& id, const std::string& options = "");
    std::shared_ptr<Descriptor> getDescriptor(const std::string& id);
    void clear() {
        m_detectors.clear();
        m_detectorGenerators.clear();
        m_descriptorGenerators.clear();
    }

private:
    DetectorFactory() {}
    DetectorFactory(const DetectorFactory&) {}
    ~DetectorFactory() {}

private:
    std::unordered_map<std::string, std::shared_ptr<Detector>> m_detectors;
    std::unordered_map<std::string, DetectorGenerator> m_detectorGenerators;
    std::unordered_map<std::string, DescriptorGenerator> m_descriptorGenerators;
};

} // end of idspace aif

#endif  // AIF_DETECTOR_FACTORY_H
