/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/face/FaceDescriptor.h>

#include <Sample.h>
#include <Renderer.h>

namespace rj = rapidjson;

namespace aif {

class FaceDetectSample : public ImageSample {
  public:
    FaceDetectSample(const std::string &configPath,
                     const std::string &defaultConfig)
        : ImageSample(configPath, defaultConfig, true), m_inputSize(0),
          m_width(0), m_height(0) {}
    virtual ~FaceDetectSample() {}

  protected:
    virtual const cv::Mat &readImage() override {
        ImageSample::readImage();
        /*m_inputSize = m_image.cols > m_image.rows ? m_image.cols : m_image.rows;
        cv::Mat padded_image;
        padded_image.create(m_inputSize, m_inputSize, m_image.type());
        padded_image.setTo(cv::Scalar::all(0));
        m_image.copyTo(
            padded_image(cv::Rect(0, 0, m_image.cols, m_image.rows)));*/

        m_width = m_image.cols;  //m_inputSize;
        m_height = m_image.rows;  //m_inputSize;
        return m_image;
    }

    virtual void onResult(std::shared_ptr<Descriptor> &descriptor) {
        std::shared_ptr<FaceDescriptor> faceDescriptor =
            std::dynamic_pointer_cast<FaceDescriptor>(descriptor);

        rj::Document json;
        json.Parse(descriptor->toStr());
        const rj::Value &faces = json["faces"];
        std::vector<cv::Rect> rects;
        for (rj::SizeType i = 0; i < faces.Size(); i++) {
            float xmin = faces[i]["region"][0].GetFloat();
            float ymin = faces[i]["region"][1].GetFloat();
            float width = faces[i]["region"][2].GetFloat();
            float height = faces[i]["region"][3].GetFloat();

            // FaceDetection 결과는 0~1사이로 정규화되므로 원래 사이즈를
            // 이용하여 박스 좌표를 계산해야 한다.
            cv::Rect rect;
            rect.x = xmin * m_width;
            rect.y = ymin * m_height;
            rect.width = width * m_width;
            rect.height = height * m_height;
            std::cout << i << ": [ " << rect.x << ", " << rect.y << ", "
                      << rect.width << ", " << rect.height << " ]" << std::endl;

            rects.push_back(rect);

        }

        Renderer::drawRects(m_image, rects, cv::Scalar(255, 0, 0), 1);
        cv::imwrite("./res.jpg", m_image);
    }

  private:
    int m_inputSize;
    int m_width;
    int m_height;
};

} // namespace aif

int main(int argc, char *argv[]) {

    std::string defaultConfig("{\n"
                              "    \"LogLevel\" : \"TRACE4\",\n"
                              "    \"ImagePath\" : \"/usr/share/aif/images/person.jpg\",\n"
                              "    \"model\" : \"face_yunet_360_640\",\n"
                              "    \"param\" : { \n"
                              "        \"common\" : {\n"
                              "            \"useXnnpack\": true,\n"
                              "            \"numThreads\": 4\n"
                              "        }\n"
                              "    }\n"
                              "}");

    std::string configPath;
    if (argc == 2) {
        if (!std::strcmp(argv[1], "--h") || !std::strcmp(argv[1], "--help")) {
            std::cout << "Usage: face-detect-sample <config-file-path>"
                      << std::endl;
            std::cout << "Example: face-detect-sample ./face.json" << std::endl;
            std::cout << "Example Config Json: " << std::endl
                      << defaultConfig << std::endl;
            ;
            return 0;
        }
        configPath = argv[1];
    }

    aif::FaceDetectSample sample(configPath, defaultConfig);
    sample.init();
    sample.run();

    return 0;
}
