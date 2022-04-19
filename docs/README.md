Overview
----
Edge AI Computer Vision is a deep learning framework for TensorFlow (e.g.TFLite) based on-device AI inference.<br>
TFLite has an ecosystem that has developed deep learning framework and supports a variety hardware accelerations such as GPU, NPU, or TPU in a mobile environment.<br>
<br>
APIs provide features below.
 * Face Detection
 * Pose Detection
 * Object Segmentation
<br> <br>

### Try your first program with Edge AI Computer Vision

Example
~~~c
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
~~~

Output
~~~json
{
  "faces": [
    {
      "score": 0.9347,
      "region": [ 0.3732, 0.1739, 0.1988, 0.1988 ],
      "lefteye": [ 0.3948, 0.2351 ],
      "righteye": [ 0.4858, 0.2253 ],
      "nosetip": [ 0.4289, 0.2753 ],
      "mouth": [ 0.4447, 0.3167 ],
      "leftear": [ 0.3805, 0.2566 ],
      "rightear": [ 0.5752, 0.233 ]
    }
  ]
}
~~~

<br><br>
