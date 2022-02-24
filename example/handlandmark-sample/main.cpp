#include <aif/sample/Sample.h>
#include <aif/handLandmark/HandLandmarkDescriptor.h>

namespace rj = rapidjson;

namespace aif {

class HandLandmarkDetectSample : public ImageSample
{
public:
    HandLandmarkDetectSample(
            const std::string& configPath,
            const std::string& defaultConfig = "")
        :ImageSample(configPath, defaultConfig, false) {}
    virtual ~HandLandmarkDetectSample() {}

protected:
    virtual void onResult(std::shared_ptr<Descriptor>& descriptor)
    {
        std::shared_ptr<HandLandmarkDescriptor> handDescriptor = 
            std::dynamic_pointer_cast<HandLandmarkDescriptor>(descriptor);
        
        rj::Document json;
        json.Parse(handDescriptor->toStr());
        const rj::Value& hands = json["hands"];
        for (rj::SizeType i = 0; i < hands.Size(); i++) {
            const rj::Value& landmark = hands[i]["landmarks"];
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
        "    \"ImagePath\" : \"./images/hand_right.jpg\",\n"
        "    \"model\" : \"handlandmark_lite_cpu\",\n"
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
            std::cout << "Usage: handlandmark-sample <config-file-path>" << std::endl;
            std::cout << "Example: handlandmark-sample ./handlandmark.json" << std::endl;
            std::cout << "Example Config Json: " << std::endl << defaultConfig << std::endl;;
            return 0;
        } 
        configPath = argv[1];
    }
    
    aif::HandLandmarkDetectSample sample(configPath, defaultConfig);
    sample.init();
    sample.run();
 
    return 0;
}

