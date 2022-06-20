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
    // std::string base64_file = "images/mona_base64.txt";
    // std::stringstream encoded_str;
    // std::ifstream fin { base64_file };
    // EXPECT_FALSE(!fin);
    // encoded_str << fin.rdbuf();
    // fin.close();

    auto encoded_str = aif::fileToStr(basePath + "/images/mona_base64.txt");
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
    std::string base64str = aif::base64Encode(basePath + "/images/mona.jpg");
    EXPECT_TRUE(base64str.size() > 0);
    Logd(base64str);
}

TEST_F(UtilTest, util04_base64Encode_to_file)
{
    bool result = aif::base64Encode(basePath + "/images/mona.jpg", basePath + "/images/mona_base64.txt");
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
        { basePath + "/images/bts.jpg", basePath + "/images/bts_decoded.jpg" },
        { basePath + "/images/blackpink.jpg", basePath + "/images/blackpink_decoded.jpg" },
        { basePath + "/images/doctors.jpg", basePath + "/images/doctos_decoded.jpg" }
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
    auto base64str = aif::fileToStr(basePath + "/images/mona_base64.txt"); // 128 x 128

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
