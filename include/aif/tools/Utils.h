/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_UTILS_H
#define AIF_UTILS_H

#include <aif/base/Types.h>
#include <aif/log/Logger.h>

#include <vector>
#include <exception>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <cfenv>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <tensorflow/lite/interpreter.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace rj = rapidjson;
namespace aif {

bool floatEquals(float a, float b);
bool floatLessThan(float a, float b);
bool floatGreaterThan(float a, float b);

std::string fileToStr(
    const std::string& file);

bool strToFile(
    const std::string& str,
    const std::string& outfile);

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

std::vector<std::string> splitString(
    const std::string& str,
    const char delim);

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
    if (interpreter == nullptr) {
        Loge("invalid interpreter!!");
        return kAifError;
    }

    if (img.rows == 0 || img.cols == 0) {
        Loge("can't load image!!");
        return kAifError;
    }

    cv::Mat img_resized;
    if (img.cols < width || img.rows < height) // expand
        cv::resize(img, img_resized, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
    else // shrink...
        cv::resize(img, img_resized, cv::Size(width, height), 0, 0, cv::INTER_AREA);

    TRACE("resized size: ", img_resized.size());
    if (img_resized.rows != height || img_resized.cols != width) {
        Loge("image resize failed!!");
        return kAifError;
    }

    if (!quantized) {
        img_resized.convertTo(img_resized, CV_32FC3);
    } else {
        img_resized.convertTo(img_resized, CV_8UC3);
    }

    if (rescaleOption == kAifNone) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                const auto &rgb = img_resized.at<CvDataType>(i, j);
                // CID 9333374, CID 9333400
                int index  = CHECK_INT_MUL(i, width);
                index = CHECK_INT_MUL(index, channels);
                int index2 = CHECK_INT_MUL(j, channels);
                index = CHECK_INT_ADD(index, index2);

                if (interpreter->typed_input_tensor<TensorDataType>(0) != nullptr) {
                    interpreter->typed_input_tensor<TensorDataType>(0)[index + 0] = rgb[2];
                    interpreter->typed_input_tensor<TensorDataType>(0)[CHECK_INT_ADD(index, 1)] = rgb[1];
                    interpreter->typed_input_tensor<TensorDataType>(0)[CHECK_INT_ADD(index, 2)] = rgb[0];
                }
            }
        }
    } else if (rescaleOption == kAifNormalization) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                const auto &rgb = img_resized.at<CvDataType>(i, j);
                // normalization: 0~1
                // CID 9333374, CID 9333400
                int index  = CHECK_INT_MUL(i, width);
                index = CHECK_INT_MUL(index, channels);
                int index2 = CHECK_INT_MUL(j, channels);
                index = CHECK_INT_ADD(index, index2);


                if (interpreter->typed_input_tensor<TensorDataType>(0) != nullptr) {
                    interpreter->typed_input_tensor<TensorDataType>(0)[index + 0] = rgb[2] / 255;
                    interpreter->typed_input_tensor<TensorDataType>(0)[CHECK_INT_ADD(index, 1)] = rgb[1] / 255;
                    interpreter->typed_input_tensor<TensorDataType>(0)[CHECK_INT_ADD(index, 2)] = rgb[0] / 255;
                }
            }
        }
    } else if (rescaleOption == kAifStandardization) {
        const float input_mean = 127.5f;
        const float input_std  = 127.5f;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                const auto &rgb = img_resized.at<CvDataType>(i, j);
                // standardization: -1~1
                // CID 9333374, CID 9333400
                int index  = CHECK_INT_MUL(i, width);
                index = CHECK_INT_MUL(index, channels);
                int index2 = CHECK_INT_MUL(j, channels);
                index = CHECK_INT_ADD(index, index2);


                if (interpreter->typed_input_tensor<TensorDataType>(0) != nullptr) {
                    interpreter->typed_input_tensor<TensorDataType>(0)[index + 0] = (rgb[2] - input_mean) / input_std;
                    interpreter->typed_input_tensor<TensorDataType>(0)[CHECK_INT_ADD(index, 1)] = (rgb[1] - input_mean) / input_std;
                    interpreter->typed_input_tensor<TensorDataType>(0)[CHECK_INT_ADD(index, 2)] = (rgb[0] - input_mean) / input_std;
                }
            }
        }
    }
    return kAifOk;
}

template<typename T>
T sigmoid(T value) {
    return 1 / (1 + std::exp(-value));
}

std::string jsonObjectToString(const rj::Value& object);
bool isIOUOver(const cv::Rect2f& cur, const cv::Rect2f& prev, float threshold);
struct t_aif_padding_info getPaddedImage(const cv::Mat& src, const cv::Size& modelSize, cv::Mat& dst,
enum cv::InterpolationFlags flag = cv::INTER_LINEAR);
bool isRoiValid( const int imgWidth, const int imgHeight, const cv::Rect &roiRect );
cv::InterpolationFlags stringToInterpolationFlags(const std::string &str);

template <typename T>
void memoryDump(T* outTensor, std::string path, size_t outTotalSize)
{

    std::ofstream outdata;
    outdata.open(path);
    if (!outdata) {
      std::cerr << "Error: file could not be opened" << std::endl;
      return;
    }

    outdata.write (reinterpret_cast<char*>(outTensor), outTotalSize);
    outdata.close();
    std::cout << "MemoryDump!!! End-of-file reached.. and size is : " << outTotalSize <<  std::endl;
    printf("[0]: 0x%2x ", outTensor[0]);
    printf("[1]: 0x%2x ", outTensor[1]);
    printf("[2]: 0x%2x \n", outTensor[2]);

    std::cout << std::dec;
}

template <typename T>
void memoryRestore(T* tensor_data, std::string path)
{
   std::ifstream indata; // indata is like cin
   uint8_t data; // variable for input value
   indata.open(path); // opens the file
   if(!indata) { // file couldn't be opened
      std::cerr << "Error: file could not be opened" << std::endl;
      return;
   }

   // get length of file:
   indata.seekg (0, std::ios::end);
   int length = indata.tellg();
   indata.seekg (0, std::ios::beg);
   std::cout << "file length is : " << length << std::endl;

   // allocate memory:
   T*buffer = new T[length];

   // read data as a block:
   indata.read (reinterpret_cast<char*>(buffer),length);

   memcpy(tensor_data, buffer, length);

   indata.close();
   std::cout << "End-of-file reached.. and size is : " << length <<  std::endl;

   delete [] buffer;
   return ;
}

std::pair<float, int>
getQuantizationTensorInfo(TfLiteTensor *tensor);

} // end of namespace aif

#endif // AIF_UTILS_H
