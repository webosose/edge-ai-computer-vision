/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Sample.h>
#include <aif/palm/PalmDescriptor.h>

namespace rj = rapidjson;

namespace aif {

class PalmDetectSample : public ImageSample
{
public:
    PalmDetectSample(
            const std::string& configPath,
            const std::string& defaultConfig = "")
        :ImageSample(configPath, defaultConfig, false) {}
    virtual ~PalmDetectSample() {}

protected:
    virtual void onResult(std::shared_ptr<Descriptor>& descriptor)
    {
        std::shared_ptr<PalmDescriptor> palmDescriptor = 
            std::dynamic_pointer_cast<PalmDescriptor>(descriptor);

        std::cout << palmDescriptor->toStr() << std::endl;        
    }
};

}

int main(int argc, char* argv[])
{
    std::string defaultConfig(
        "{\n"
        "    \"LogLevel\" : \"TRACE4\",\n"
        "    \"ImagePath\" : \"./images/hand_right.jpg\",\n"
        "    \"model\" : \"palm_lite_cpu\",\n"
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
            std::cout << "Usage: palm-sample <config-file-path>" << std::endl;
            std::cout << "Example: palm-sample ./palm.json" << std::endl;
            std::cout << "Example Config Json: " << std::endl << defaultConfig << std::endl;;
            return 0;
        } 
        configPath = argv[1];
    }
    
    aif::PalmDetectSample sample(configPath, defaultConfig);
    sample.init();
    sample.run();
 
    return 0;
}

