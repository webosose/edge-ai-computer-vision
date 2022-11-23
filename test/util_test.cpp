/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <boost/beast/core/detail/base64.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace boost::beast::detail;
using aif::Logger;

class UtilTest : public ::testing::Test
{
protected:
    UtilTest() = default;
    ~UtilTest() = default;

    static void SetUpTestCase()
    {
        aif::AIVision::init();
    }

    static void TearDownTestCase()
    {
        aif::AIVision::deinit();
    }


    void SetUp() override
    {
        Logger::init();
        basePath = aif::AIVision::getBasePath();
    }

    void TearDown() override
    {
    }

    std::string basePath;
};

TEST_F(UtilTest, util01_base64_encode)
{
    std::string image_file = basePath + "/images/mona.jpg";
    std::ifstream fin { image_file, std::ifstream::binary };
    EXPECT_FALSE(!fin);
    fin.seekg(0, fin.end);
    std::size_t length = static_cast<std::size_t>(fin.tellg());
    fin.seekg(0, fin.beg);
    std::vector<char> image_data(length);
    fin.read(image_data.data(), length);

    auto encoded_size = base64::encoded_size(length);
    std::vector<char> buffer(encoded_size);
    Logd("encoded_size: ", encoded_size);

    auto result = base64::encode(buffer.data(), image_data.data(), length);
    EXPECT_TRUE(result > 0);
    std::string encoded_str(buffer.begin(), buffer.end());
    Logd("encoded_str: ", encoded_str);
    Logd("result: ", result);


    fin.close();
}

TEST_F(UtilTest, util02_base64_decode)
{
    // std::string base64_file = "images/mona_base64.jpg";
    // std::stringstream encoded_str;
    // std::ifstream fin { base64_file };
    // EXPECT_FALSE(!fin);
    // encoded_str << fin.rdbuf();
    // fin.close();

    auto encoded_str = aif::fileToStr(basePath + "/images/mona_base64.jpg");
    Logd(encoded_str);

    auto decoded_size = base64::decoded_size(encoded_str.size());
    Logd("decoded_size: ", decoded_size);

    std::vector<char> buffer(decoded_size);
    auto result = base64::decode(buffer.data(), encoded_str.data(), encoded_str.size());

    Logd("out.len: ", result.first);
    Logd("input.len: ", result.second);
    EXPECT_LE(result.first, buffer.size());

    // std::ofstream imgFile("images/mona_decoded.jpg", std::ios::out| std::ios::binary);
    // EXPECT_FALSE(!imgFile);
    // imgFile.write(buffer.data(), result.first);
    // imgFile.close();

    EXPECT_TRUE(aif::bufferToFile(buffer, result.first, basePath + "/images/mona_decoded.jpg"));
}

TEST_F(UtilTest, util03_base64Encode_to_string)
{
    std::string base64str_xxx = aif::base64Encode(basePath + "/images/xxx.jpg"); /* file not exist */
    EXPECT_EQ(base64str_xxx, "");

    std::string base64str = aif::base64Encode(basePath + "/images/mona.jpg");
    EXPECT_TRUE(base64str.size() > 0);
    Logd(base64str);
}

TEST_F(UtilTest, util04_base64Encode_to_file)
{
    bool result = aif::base64Encode(basePath + "/images/mona.jpg", basePath + "/images/mona_base64.jpg");
    EXPECT_TRUE(result);
}

TEST_F(UtilTest, util05_base64Decode_to_buffer)
{
    std::string base64str = aif::base64Encode(basePath + "/images/mona.jpg");
    EXPECT_TRUE(base64str.size() > 0);
    //Logd(base64str);

    int dataSize = 0;
    auto data = aif::base64Decode(base64str, dataSize);
    EXPECT_TRUE(dataSize> 0);
    EXPECT_GE(data.size(), dataSize);
    Logd("data.size(): ", data.size(), " vs. dataSize: ", dataSize);
}

TEST_F(UtilTest, util06_base64Decode_to_file)
{
    std::array<std::pair<std::string, std::string>, 4> filenames{{
        { basePath + "/images/mona.jpg", basePath + "/images/mona_decoded.jpg" },
        { basePath + "/images/5faces.jpg", basePath + "/images/5faces_decoded.jpg" },
        { basePath + "/images/2faces.jpg", basePath + "/images/2faces_decoded.jpg" },
        { basePath + "/images/5people.jpg", basePath + "/images/5people_decoded.jpg" }
    }};

    for (const auto& f : filenames) {

        std::string base64str = aif::base64Encode(f.first);
        EXPECT_TRUE(base64str.size() > 0);

        //Logd(base64str);

        bool result = aif::base64Decode(base64str, f.second);
        EXPECT_TRUE(result);
    }
}

TEST_F(UtilTest, util07_base64Decode_normalize)
{
    auto base64str = aif::fileToStr(basePath + "/images/mona_base64.jpg"); // 128 x 128

    const int width = 128;
    const int height = 128;
    const int channels = 3;
    int imgDataSize = height * width * channels;
    std::vector<float> imgData(imgDataSize);
    auto result = aif::normalizeWithBase64Image(
        base64str,
        width,
        height,
        channels,
        imgData
    );
    EXPECT_TRUE(result);
}

TEST_F(UtilTest, util08_image_normalize)
{
    auto imagePathstr = basePath + "/images/mona.jpg";    // 200 x 298
    const int width = 200;
    const int height = 298;
    const int channels = 3;
    int imgDataSize = height * width * channels;
    std::vector<float> imgData(imgDataSize);
    auto result = aif::normalizeWithImage(
        imagePathstr,
        width,
        height,
        channels,
        imgData
    );
    EXPECT_TRUE(result);
}

TEST_F(UtilTest, util09_floatEquals)
{
    EXPECT_TRUE(aif::floatEquals(0.001, 0.001));
    EXPECT_FALSE(aif::floatEquals(0.001, 0.003));
    EXPECT_FALSE(aif::floatEquals(0.003, 0.001));
}

TEST_F(UtilTest, util10_getCvImage)
{
    cv::Mat cvImg;
    auto ret = aif::getCvImageFrom(basePath + "/images/mona.jpg", cvImg);
    EXPECT_EQ(ret, aif::kAifOk);

    auto base64str = aif::fileToStr(basePath + "/images/mona_base64.jpg"); // 128 x 128
    ret = aif::getCvImageFromBase64(base64str, cvImg);
    EXPECT_EQ(ret, aif::kAifOk);
}

TEST_F(UtilTest, util11_splitString)
{
    std::string str = ":abc:de:f:";

    auto strings = aif::splitString(str, ':');

    EXPECT_EQ(strings.size(), 4);

    EXPECT_EQ(strings[0], "");
    EXPECT_EQ(strings[1], "abc");
    EXPECT_EQ(strings[2], "de");
    EXPECT_EQ(strings[3], "f");
}

TEST_F(UtilTest, util12_isIOUOver)
{
    /* x, y, w, h */
    cv::Rect2f rect1(1.0, 3.0, 4.0, 6.0);
    cv::Rect2f rect2(3.0, 5.0, 6.0, 6.0);
    cv::Rect2f rect3(1.1, 2.9, 3.9, 5.9);
    cv::Rect2f rect4(5.0, 3.0, 2.0, 3.0);

    EXPECT_FALSE(aif::isIOUOver(rect1, rect2, 0.7f));
    EXPECT_FALSE(aif::isIOUOver(rect2, rect1, 0.7f));
    EXPECT_TRUE(aif::isIOUOver(rect1, rect3, 0.7f));
    EXPECT_TRUE(aif::isIOUOver(rect3, rect1, 0.7f));
    EXPECT_FALSE(aif::isIOUOver(rect1, rect4, 0.7f));

}
