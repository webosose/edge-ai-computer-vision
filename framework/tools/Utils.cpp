/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <boost/beast/core/detail/base64.hpp>

// #include <opencv2/opencv.hpp>
// #include <opencv2/imgproc.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

using namespace boost::beast::detail;
using namespace aif;

namespace {

static const float INPUT_MEAN = 127.5f;
static const float INPUT_STD = 127.5f;

} // end of anonymous namespace
namespace aif {

bool floatEquals(float a, float b)
{
    return (std::abs(a-b) < EPSILON);
}

std::string fileToStr(const std::string& file)
{
    std::stringstream buffer;
    std::ifstream fin { file };
    if (fin) {
        buffer << fin.rdbuf();
        fin.close();
        return buffer.str();
    } else {
        return "";
    }
}

bool bufferToFile(const std::vector<char>& buffer, int len, const std::string& outfile)
{
    std::ofstream fout(outfile, std::ios::out | std::ios::binary);
    if (!fout) {
        Loge(__func__, "Error: can't write file at ", outfile);
        return false;
    }
    fout.write(buffer.data(), len);
    fout.close();
    return true;
}

std::string base64Encode(const std::string& inputfile)
{
    std::ifstream fin;
    std::stringstream errlog;
    try {
        fin.open(inputfile);
        if (!fin.is_open()) {
            errlog.clear();
            errlog << "file not found: " << inputfile;
            throw std::runtime_error(errlog.str());
        }

        fin.seekg(0, fin.end);
        std::size_t data_length = static_cast<std::size_t>(fin.tellg());
        fin.seekg(0, fin.beg);

        std::vector<char> data_buffer(data_length);
        fin.read(data_buffer.data(), data_length);

        auto encoded_size = base64::encoded_size(data_length);
        std::vector<char> encode_buffer(encoded_size);
        // std::cout << "encoded_size: " << encoded_size << std::endl;

        auto result = base64::encode(encode_buffer.data(), data_buffer.data(), data_length);
        std::string encoded_str(encode_buffer.begin(), encode_buffer.end());
        // std::cout << encoded_str << std::endl;
        // std::cout << "result: " << result << std::endl;
        fin.close();

        return encoded_str;
    } catch(std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        if (fin.is_open())
            fin.close();
        return "";
    } catch(...) {
        Loge(__func__, "Error: Unknown error!!");
        if (fin.is_open())
            fin.close();
        return "";
    }
}

bool base64Encode(const std::string& inputfile, const std::string& outfile)
{
    std::ofstream fout;
    std::stringstream errlog;
    try {
        std::string encoded_str = base64Encode(inputfile);
        if (encoded_str.empty()) {
            errlog.clear();
            errlog << "base64encode error: " << inputfile << std::endl;
            throw std::runtime_error(errlog.str());
        }

        fout.open(outfile);
        if (!fout.is_open()) {
            errlog.clear();
            errlog << "file open error: " << outfile << std::endl;
            throw std::runtime_error(errlog.str());
        }
        fout.write(encoded_str.data(), encoded_str.size());
        fout.close();
        return true;
    } catch(std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        if (fout.is_open())
            fout.close();
        return false;
    } catch(...) {
        Loge(__func__, "Error: Unknown error!!");
        if (fout.is_open())
            fout.close();
        return false;
    }
}

std::vector<char> base64Decode(const std::string& base64str, int& dataSize)
{
    auto decoded_size = base64::decoded_size(base64str.size());
    // std::cout << "decoded_size: " << decoded_size << std::endl;

    std::vector<char> buffer(decoded_size);
    auto result = base64::decode(buffer.data(), base64str.data(), base64str.size());

    // std::cout << "out.len: " << result.first << std::endl;
    // std::cout << "input.len: " << result.second << std::endl;
    // std::cout << "buffer data size: " << buffer.size() << std::endl;

    dataSize = result.first;
    return buffer;
}

bool base64Decode(const std::string& base64str, const std::string& outfile)
{
    std::ofstream fout;
    std::stringstream errlog;
    try {
        int decoded_size = 0;
        auto decoded_data = base64Decode(base64str, decoded_size);
        if (decoded_size == 0) {
            throw std::runtime_error("base64Decode error!!");
        }

        fout.open(outfile);
        if (!fout.is_open()) {
            errlog.clear();
            errlog << "file open error: " << outfile << std::endl;
            throw std::runtime_error(errlog.str());
        }
        fout.write(decoded_data.data(), decoded_size);
        fout.close();
        return true;
    } catch(std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        if (fout.is_open())
            fout.close();
        return false;
    } catch(...) {
        Loge(__func__, "Error: Unknown error!!");
        if (fout.is_open())
            fout.close();
        return false;
    }
}

// resize and normalize
bool normalizeWithImage(
    const std::string& imgPath,
    int width,
    int height,
    int channels,
    std::vector<float>& imgData)
{
    std::ofstream fout;
    std::stringstream errlog;
    try {
        cv::Mat img = cv::imread(imgPath.c_str(), cv::IMREAD_COLOR);
        if (img.rows <= 0 || img.cols <=0) {
            errlog.clear();
            errlog << "cv::imread failed: " << imgPath;
            throw std::runtime_error(errlog.str());
        }

        cv::Mat img_resized;
        cv::resize(img, img_resized, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
        //EXPECT_TRUE(img_resized.rows == height);
        //EXPECT_TRUE(img_resized.cols == width);
        if (img_resized.rows != height || img_resized.cols != width) {
            throw std::runtime_error("cv::resize failed!!");
        }

        cv::Mat img_resized_rgb;
        cv::cvtColor(img_resized, img_resized_rgb, cv::COLOR_BGR2RGB);
        img_resized_rgb.convertTo(img_resized_rgb, CV_32F);

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                const auto &rgb = img_resized_rgb.at<cv::Vec3f>(i, j);
                // normalization: 0~1
                imgData[i * width * channels + j * channels + 0] = (rgb[0] - INPUT_MEAN) / INPUT_STD;
                imgData[i * width * channels + j * channels + 1] = (rgb[1] - INPUT_MEAN) / INPUT_STD;
                imgData[i * width * channels + j * channels + 2] = (rgb[2] - INPUT_MEAN) / INPUT_STD;
            }
        }

        // EXPECT_EQ(imgData.size(), height*width*channels);
        if (imgData.size() != height*width*channels) {
            throw std::runtime_error("normalize failed!!");
        }
        return true;
    } catch(std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        return false;
    } catch(...) {
        Loge(__func__, "Error: Unknown error!!");
        return false;
    }
}

// only normalize
bool normalizeWithBase64Image(
    const std::string& base64str,
    int width,
    int height,
    int channels,
    std::vector<float>& imgData)
{
    try {
        int decoded_size = 0;
        auto decoded_image = base64Decode(base64str, decoded_size);
        if (decoded_image.size() == 0 || decoded_size == 0) {
            throw std::runtime_error("base64Decode failed!!");
        }
        TRACE("base64 decoded image size: ", decoded_size);
        //cv::Mat img_resized(height, width, CV_8UC3, (char*)decoded_image.data());
        cv::Mat img_resized = cv::imdecode(cv::Mat(decoded_image), 1);
        // EXPECT_TRUE(img.rows > 0 && img.cols > 0);
        // cv::Mat img_resized;
        // cv::resize(img, img_resized, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
        // EXPECT_TRUE(img_resized.rows == height);
        // EXPECT_TRUE(img_resized.cols == width);
        if (img_resized.rows != height || img_resized.cols != width) {
            std::runtime_error("can't create cv image from array!!");
        }

        cv::Mat img_resized_rgb;
        cv::cvtColor(img_resized, img_resized_rgb, cv::COLOR_BGR2RGB);
        img_resized_rgb.convertTo(img_resized_rgb, CV_32F);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                const auto &rgb = img_resized_rgb.at<cv::Vec3f>(i, j);
                // normalization: 0~1
                imgData[i * width * channels + j * channels + 0] = (rgb[0] - INPUT_MEAN) / INPUT_STD;
                imgData[i * width * channels + j * channels + 1] = (rgb[1] - INPUT_MEAN) / INPUT_STD;
                imgData[i * width * channels + j * channels + 2] = (rgb[2] - INPUT_MEAN) / INPUT_STD;
            }
        }
        // EXPECT_EQ(imgData.size(), height*width*channels);
        if (imgData.size() != height*width*channels) {
            throw std::runtime_error("normalize failed!!");
        }
        return true;
    } catch(std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        return false;
    } catch(...) {
        Loge(__func__, "Error: Unknown error!!");
        return false;
    }
}

t_aif_status getCvImageFrom(
    const std::string& imagePath,
    cv::Mat& cvImg)
{
    cvImg = cv::imread(imagePath.c_str(), cv::IMREAD_COLOR);
    return (cvImg.rows > 0 && cvImg.cols > 0) ? kAifOk : kAifError;
}

t_aif_status getCvImageFromBase64(
    const std::string& base64image,
    cv::Mat& cvImg)
{
    int decoded_size = 0;
    auto decoded_image = base64Decode(base64image, decoded_size);
    if (decoded_image.size() == 0 || decoded_size == 0) {
        Loge(__func__, "Error: base64image decode failed!!");
        return kAifError;
    }
    TRACE("base64 decoded image size: ", decoded_size);
    //cv::Mat img_resized(height, width, CV_8UC3, (char*)decoded_image.data());
    cvImg = cv::imdecode(cv::Mat(decoded_image), 1);
    return (cvImg.rows > 0 && cvImg.cols > 0) ? kAifOk : kAifError;
}

std::vector<std::string> splitString(
    const std::string& str,
    const char delim)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ss(str);
    while (std::getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string jsonObjectToString(const rj::Value& object)
{
    rj::StringBuffer strbuf;
    strbuf.Clear();

    rj::Writer<rj::StringBuffer> writer(strbuf);
    object.Accept(writer);
    return strbuf.GetString();
}


bool isIOUOver(const cv::Rect2f& cur, const cv::Rect2f& prev, float threshold)
{
    float A = cur.width * cur.height;
    float B = prev.width * prev.height;
    float x1, y1, w1, h1, x2, y2, w2, h2;
    if (cur.x < prev.x) {
        x1 = cur.x;
        w1 = cur.width;
        x2 = prev.x;
        w2 = prev.width;
    } else {
        x2 = cur.x;
        w2 = cur.width;
        x1 = prev.x;
        w1 = prev.width;
    }

    if (cur.y < prev.y) {
        y1 = cur.y;
        h1 = cur.height;
        y2 = prev.y;
        h2 = prev.height;
    } else {
        y2 = cur.y;
        h2 = cur.height;
        y1 = prev.y;
        h1 = prev.height;
    }

    float A_and_B =
        (std::min(x1 + w1, x2 + w2) - x2) *
        (std::min(y1 + h1, y2 + h2) - y2);
    float A_or_B = A + B - A_and_B;

    TRACE("IOU : ", (A_and_B / A_or_B));
    return ((A_and_B / A_or_B) > threshold);
}

} // end of namespace aif
