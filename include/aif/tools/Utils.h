#ifndef AIF_UTILS_H
#define AIF_UTILS_H

#include <aif/base/Types.h>
#include <aif/log/Logger.h>

#include <vector>
#include <exception>
#include <cmath>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <tensorflow/lite/interpreter.h>

namespace aif {

bool floatEquals(
    float a,
    float b);

std::string fileToStr(
    const std::string& file);

bool bufferToFile(
    const std::vector<char>& buffer,
    int len,
    const std::string& outfile);

std::string base64Encode(
    const std::string& inputfile);

bool base64Encode(
    const std::string& inputfile,
    const std::string& outfile);

std::vector<char> base64Decode(
    const std::string& base64str,
    int& dataSize);

bool base64Decode(
    const std::string& base64str,
    const std::string& outfile);

bool normalizeWithImage(
    const std::string& imgPath,
    int width,
    int height,
    int channels,
    std::vector<float>& imgData);

bool normalizeWithBase64Image(
    const std::string& base64str,
    int width,
    int height,
    int channels,
    std::vector<float>& imgData);

t_aif_status getCvImageFrom(
    const std::string& imagePath,
    cv::Mat& cvImg);

t_aif_status getCvImageFromBase64(
    const std::string& base64image,
    cv::Mat& cvImg);

template<typename TensorDataType, typename CvDataType>
t_aif_status fillInputTensor(
    tflite::Interpreter* interpreter,
    const cv::Mat& img,
    int width,
    int height,
    int channels,
    bool quantized,
    t_aif_rescale rescaleOption)
{
    try {

        if (interpreter == nullptr) {
            throw std::runtime_error("invalid interpreter!!");
        }

        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("can't load image!!");
        }

        cv::Mat img_resized;
        if (img.cols < width || img.rows < height) // expand
            cv::resize(img, img_resized, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
        else // shrink...
            cv::resize(img, img_resized, cv::Size(width, height), 0, 0, cv::INTER_AREA);

        TRACE("resized size: ", img_resized.size());
        if (img_resized.rows != height || img_resized.cols != width) {
            throw std::runtime_error("image resize failed!!");
        }

        if (!quantized) {
            img_resized.convertTo(img_resized, CV_32F);
        } else {
            img_resized.convertTo(img_resized, CV_8U);
        }

        if (rescaleOption == kAifNone) {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    const auto &rgb = img_resized.at<CvDataType>(i, j);
                    interpreter->typed_input_tensor<TensorDataType>(0)[i * width * channels + j * channels + 0] = rgb[2];
                    interpreter->typed_input_tensor<TensorDataType>(0)[i * width * channels + j * channels + 1] = rgb[1];
                    interpreter->typed_input_tensor<TensorDataType>(0)[i * width * channels + j * channels + 2] = rgb[0];
                }
            }
        } else if (rescaleOption == kAifNormalization) {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    const auto &rgb = img_resized.at<CvDataType>(i, j);
                    // normalization: 0~1
                    interpreter->typed_input_tensor<TensorDataType>(0)[i * width * channels + j * channels + 0] = rgb[2] / 255;
                    interpreter->typed_input_tensor<TensorDataType>(0)[i * width * channels + j * channels + 1] = rgb[1] / 255;
                    interpreter->typed_input_tensor<TensorDataType>(0)[i * width * channels + j * channels + 2] = rgb[0] / 255;
                }
            }
        } else if (rescaleOption == kAifStandardization) {
            const float input_mean = 127.5f;
            const float input_std  = 127.5f;

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    const auto &rgb = img_resized.at<CvDataType>(i, j);
                    // standardization: -1~1
                    interpreter->typed_input_tensor<TensorDataType>(0)[i * width * channels + j * channels + 0] = (rgb[2] - input_mean) / input_std;
                    interpreter->typed_input_tensor<TensorDataType>(0)[i * width * channels + j * channels + 1] = (rgb[1] - input_mean) / input_std;
                    interpreter->typed_input_tensor<TensorDataType>(0)[i * width * channels + j * channels + 2] = (rgb[0] - input_mean) / input_std;
                }
            }
        }
        return kAifOk;
    } catch(std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__, "Error: Unknow exception occured!!");
        return kAifError;
    }
}

template<typename T>
T sigmoid(T value) {
    return 1 / (1 + std::exp(-value));
}

} // end of namespace aif

#endif // AIF_UTILS_H
