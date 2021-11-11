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

static std::shared_ptr<aif::Detector> posenetDetector = nullptr;

} // anonymous namespace

#ifdef USE_ARMNN
const char* DETECTOR_NAME  = "posenet_mobilenet_armnn";
#else
const char* DETECTOR_NAME  = "posenet_mobilenet_cpu";
#endif

void DetectPosenet(const std::string& image_path, std::vector<cv::Rect>& poseRects)
{
    std::shared_ptr<aif::Descriptor> descriptor = aif::DetectorFactory::get().getDescriptor(DETECTOR_NAME);
    posenetDetector->detectFromImage(image_path, descriptor);

    //std::cout << descriptor->toStr() << std::endl;
    rj::Document json;
    json.Parse(descriptor->toStr());
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

void printUsage()
{
    std::cerr <<
        "Usage: posenet-sample --model ${path to model} --image ${path to image}\n" <<
        "       <Optional> --useArmNN \"backends:CpuRef,CpuAcc;logging-severity:info\"\n"<<
        "Example:\n" <<
        "    posenet-sample --model models/posenet_mobilenet_v1_075_353_481_quant_decoder.tflite " <<
                            "--image images/surf.jpg --useArmNN " <<
                            "\"backends:CpuRef,CpuAcc;logging-severity:info\"\n";
}

bool validateParams(int argc, char* argv[], bool& useArmNN, std::string& armnnOptions)
{
    if (argc < 5) {
        printUsage();
        return false;
    }

    useArmNN = false;

    if (std::strcmp(argv[1], "--model") || std::strcmp(argv[3], "--image"))
    {
        printUsage();
        return false;
    }

    if (argc >= 6)
    {
        if (!std::strcmp(argv[5], "--useArmNN"))
        {
            useArmNN = true;
            if (argc == 7) {
                armnnOptions = argv[6];
            }
            return true;
        }
        else {
            printUsage();
            return false;
        }
    }

    return true;
}

int main(int argc, char* argv[])
{
    bool useArmnn = false;
    std::string armnnOptions("");
    if (!validateParams(argc, argv, useArmnn, armnnOptions)) {
        return EXIT_FAILURE;
    }

    const std::string model_path = argv[2];
    const std::string image_path = argv[4];

    std::cout << "imagefile: " << image_path << std::endl;

    if (useArmnn) {
        //armnnOptions = "backends:CpuAcc;logging-severity:info";
        std::cout << "armnnOptions: " << armnnOptions << std::endl;
        posenetDetector = aif::DetectorFactory::get().getDetector(DETECTOR_NAME);
    } else {
        posenetDetector = aif::DetectorFactory::get().getDetector(DETECTOR_NAME);
    }

    if (posenetDetector == nullptr) {
        std::cerr << "posenetDetector create error!" << std::endl;
        return EXIT_FAILURE;
    }

    if (posenetDetector->init() != kAifOk) {
        std::cerr << "posenetDetector init error!" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<cv::Rect> posenets;
    DetectPosenet(image_path, posenets);

    return EXIT_SUCCESS;
}
