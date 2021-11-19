#include <aif/base/DetectorFactory.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <vector>

using namespace aif;
namespace rj = rapidjson;

namespace {

static int s_width = 0;
static int s_height = 0;
static int s_bpp = 0;

static std::shared_ptr<aif::Detector> poselandmarkDetector = nullptr;
static std::string DETECTOR_NAME = "";

} // anonymous namespace

void DetectPoselandmark(const std::string& image_path, std::vector<cv::Rect>& poseRects)
{
    std::shared_ptr<aif::Descriptor> descriptor = aif::DetectorFactory::get().getDescriptor(DETECTOR_NAME);
    poselandmarkDetector->detectFromImage(image_path, descriptor);

    //std::cout << descriptor->toStr() << std::endl;
    rj::Document json;
    json.Parse(descriptor->toStr());
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

void printUsage()
{
    std::cerr <<
        "Usage: poselandmark-sample --image ${path to image} ${lite/full/heavy}\n" <<
        "       <Optional> --useXnnpack=${true or false} ${numThreads}\n" <<
        "Example:\n" <<
        "    poselandmark-sample --image images/person.jpg lite \n" <<
        "                   --useXnnpack=true 4\n";
}

bool validateParams(int argc, char* argv[], std::string& options)
{
    if (argc < 4) {
        printUsage();
        return false;
    }

    if (std::strcmp(argv[1], "--image"))
    {
        printUsage();
        return false;
    }


    if (argc >= 5)
    {
#ifdef USE_XNNPACK
        if (std::strstr(argv[4], "--useXnnpack") != nullptr)
        {
            auto key_value = aif::splitString(argv[4], '=');
            if (key_value.size() != 2) {
                printUsage();
                return false;
            }

            options.append(key_value[1]);
            if (argc == 6) {
                options.append(";");
                options.append(argv[5]);
            }

            return true;
        }
#endif
        printUsage();
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    Logger::init(aif::LogLevel::TRACE1);

    std::string options("");
    if (!validateParams(argc, argv, options)) {
        return EXIT_FAILURE;
    }

    const std::string image_path = argv[2];
    const std::string modelType = argv[3];

    std::cout << "imagefile: " << image_path << std::endl;
    std::cout << "modelType: " << modelType << std::endl;

    std::cout << "Options: " << options << std::endl;

    if (!modelType.compare("lite")){
        DETECTOR_NAME = "poselandmark_lite_cpu";
    } else if (!modelType.compare("full")){
        DETECTOR_NAME = "poselandmark_full_cpu";
    } else if (!modelType.compare("heavy")){
        DETECTOR_NAME = "poselandmark_heavy_cpu";
    } else {
        std::cerr << "Wrong model Type...: " << modelType << std::endl;
        return EXIT_FAILURE;
    }

    poselandmarkDetector = aif::DetectorFactory::get().getDetector(DETECTOR_NAME, options);

    if (poselandmarkDetector == nullptr) {
        std::cerr << "poselandmarkDetector create error!" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<cv::Rect> poselandmarks;
    DetectPoselandmark(image_path, poselandmarks);

    return EXIT_SUCCESS;
}
