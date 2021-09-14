#include <aif/face/FaceDetectorFactory.h>
#include <aif/face/CpuFaceDetector.h>
#ifdef USE_EDGETPU
#include <aif/face/EdgeTpuFaceDetector.h>
#endif

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

static std::shared_ptr<aif::FaceDetector> faceDetector = nullptr;

} // anonymous namespace

void DetectFaces(const cv::Mat& frame, std::vector<cv::Rect>& faceRects)
{
    std::shared_ptr<aif::Descriptor> descriptor = std::make_shared<aif::FaceDescriptor>();
    faceDetector->detect(frame, descriptor);
    /*
    {
        "faces": [
            {
                "score":0.9347445368766785,
                "region":[0.37328392267227175,0.17393717169761659,0.5720989108085632,0.37275126576423647],
                "lefteye":[0.3948841094970703,0.23510795831680299],
                "righteye":[0.48589372634887698,0.2253197431564331],
                "nosetip":[0.4289666712284088,0.27532318234443667],
                "mouth":[0.44470444321632388,0.3167915940284729],
                "leftear":[0.3805391788482666,0.2566477358341217],
                "rightear":[0.5752524137496948,0.23307925462722779]
            }
        ]
    }
    */
    //std::cout << descriptor->toStr() << std::endl;
    rj::Document json;
    json.Parse(descriptor->toStr());
    const rj::Value& faces = json["faces"];
    for (rj::SizeType i = 0; i < faces.Size(); i++) {
        double xmin = faces[i]["region"][0].GetDouble();
        double ymin = faces[i]["region"][1].GetDouble();
        double xmax = faces[i]["region"][2].GetDouble();
        double ymax = faces[i]["region"][3].GetDouble();

        // FaceDetection 결과는 0~1사이로 정규화되므로 원래 사이즈를 이용하여 박스 좌표를 계산해야 한다.
        cv::Rect rect;
        rect.x = xmin * s_width;
        rect.y = ymin * s_height;
        rect.width = (xmax - xmin) * s_width;
        rect.height = (ymax - ymin) * s_height;
        faceRects.push_back(rect);
    }
}

int main(int argc, char* argv[])
{
    // ConfigReader reader("aif.config");
    // Logger::init(Logger::strToLogLevel(reader.getOption("LogLevel")));
    // Logd("**face_websocket server**");
    // Check command line arguments.
    if (argc != 2)
    {
        std::cerr <<
            "Usage: face-detect-sample <image-file>\n" <<
            "Example:\n" <<
            "    face-detect-sample images/mona.jpg\n";
        return EXIT_FAILURE;
    }

    auto const imagefile = argv[1];

    std::cout << "imagefile: " << imagefile << std::endl;

    cv::Mat image = cv::imread(imagefile);

    s_width = image.cols;
    s_height = image.rows;
    s_bpp = image.channels();
    int imagesize = s_width * s_height * s_bpp;
    std::cout << imagefile << ": w=" <<  s_width <<
        ", h=" << s_height <<
        ", bpp=" << s_bpp <<
        ", imagesize=" << imagesize << std::endl;

    faceDetector = aif::FaceDetectorFactory::create("short_range");
    if (faceDetector == nullptr) {
        std::cerr << "faceDetector create error!" << std::endl;
        return EXIT_FAILURE;
    }

    if (faceDetector->init() != kAifOk) {
        std::cerr << "faceDetector init error!" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<cv::Rect> faces;
    DetectFaces(image, faces);

    for(auto i = 0; i < faces.size(); i++) {
        std::cout << i << ": [ "
            << faces[i].x << ", "
            << faces[i].y << ", "
            << faces[i].width << ", "
            << faces[i].height << " ]"
            << std::endl;
    }

    return EXIT_SUCCESS;
}
