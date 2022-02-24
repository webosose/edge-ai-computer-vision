#include <aif/sample/Sample.h>
#include <aif/poseLandmark/PoseLandmarkDescriptor.h>

namespace rj = rapidjson;

namespace aif {

class PoseLandmarkDetectSample : public ImageSample
{
public:
    PoseLandmarkDetectSample(
            const std::string& configPath,
            const std::string& defaultConfig = "")
        :ImageSample(configPath, defaultConfig, false) {}
    virtual ~PoseLandmarkDetectSample() {}

protected:
    virtual void onResult(std::shared_ptr<Descriptor>& descriptor)
    {
        std::shared_ptr<PoseLandmarkDescriptor> poseDescriptor = 
            std::dynamic_pointer_cast<PoseLandmarkDescriptor>(descriptor);
        
        rj::Document json;
        json.Parse(poseDescriptor->toStr());
        const rj::Value& poses = json["poses"];
        for (rj::SizeType i = 0; i < poses.Size(); i++) {
            const rj::Value& landmark = poses[i]["landmarks"];
            std::cout << "landmarks: " << std::endl;

            for (rj::SizeType j = 0; j < landmark.Size(); j++) {
                for (rj::SizeType k = 0; k < landmark[j].Size(); k++) {
                    std::cout << landmark[j][k].GetDouble() << ", ";
                }
                std::cout << "\n";
            }
        }
    }
};

}

int main(int argc, char* argv[])
{
    std::string defaultConfig(
        "{\n"
        "    \"LogLevel\" : \"TRACE4\",\n"
        "    \"ImagePath\" : \"./images/person.jpg\",\n"
        "    \"model\" : \"poselandmark_lite_cpu\",\n"
        "    \"param\" : { \n"
        "        \"common\" : {\n"
        "            \"useXnnpack\": true,\n"
        "            \"numThreads\": 4\n"
        "        }\n"
        "    }\n"
        "}"
    );
    
    std::string configPath;
    if (argc == 2) {
        if (!std::strcmp(argv[1], "--h") || !std::strcmp(argv[1], "--help")) {
            std::cout << "Usage: poselandmark-sample <config-file-path>" << std::endl;
            std::cout << "Example: poselandmark-sample ./poselandmark.json" << std::endl;
            std::cout << "Example Config Json: " << std::endl << defaultConfig << std::endl;;
            return 0;
        } 
        configPath = argv[1];
    }
    
    aif::PoseLandmarkDetectSample sample(configPath, defaultConfig);
    sample.init();
    sample.run();
 
    return 0;
}

