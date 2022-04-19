#include <aif/facade/EdgeAIVision.h>

using namespace aif;

int main()
{
    cv::Mat input = cv::imread("/usr/share/aif/images/mona.jpg", cv::IMREAD_COLOR);
    std::string output;

    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::FACE;
    EdgeAIVision ai = EdgeAIVision::getInstance();

    ai.startup();
    ai.createDetector(type);
    ai.detect(type, input, output);
    ai.deleteDetector(type);
    ai.shutdown();

    std::cout << output << std:: endl;
    return 0;
}
