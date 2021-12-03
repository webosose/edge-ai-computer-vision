#include <aif/sample/Sample.h>
#include <aif/pose/PosenetDescriptor.h>

namespace rj = rapidjson;

namespace aif {

class PoseDetectSample : public ImageSample
{
public:
    PoseDetectSample(
            const std::string& configPath,
            const std::string& defaultConfig = "")
        :ImageSample(configPath, defaultConfig) {}
    virtual ~PoseDetectSample() {}

protected:
    virtual void onResult(std::shared_ptr<Descriptor>& descriptor)
    {
        std::shared_ptr<PosenetDescriptor> poseDescriptor = 
            std::dynamic_pointer_cast<PosenetDescriptor>(descriptor);

        rj::Document json;
        json.Parse(poseDescriptor->toStr());
        const rj::Value& poses = json["poses"];
        for (rj::SizeType i = 0; i < poses.Size(); i++) {
            auto score = poses[i]["score"].GetDouble();
            const rj::Value& keypoints = poses[i]["keyPoints"];

            std::cout << "keyPoints: " << std::endl;
            for (rj::SizeType j = 0; j < keypoints.Size(); j++) {
                auto point_x = keypoints[j][0].GetDouble();
                auto point_y = keypoints[j][1].GetDouble();
                std::cout << "        (" << point_x << ", " << point_y << ")\n";
            }
            std::cout << "score: " << score << std::endl;
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
        "    \"model\" : \"posenet_mobilenet_cpu\",\n"
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
            std::cout << "Usage: posenet-sample <config-file-path>" << std::endl;
            std::cout << "Example: posenet-sample ./pose.json" << std::endl;
            std::cout << "Example Config Json: " << std::endl << defaultConfig << std::endl;;
            return 0;
        } 
        configPath = argv[1];
    }
    
    aif::PoseDetectSample sample(configPath, defaultConfig);
    sample.init();
    sample.run();
    
    return 0;
}

