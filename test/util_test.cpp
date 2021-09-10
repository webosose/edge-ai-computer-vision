#include <aif/base/Utils.h>
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

    void SetUp() override
    {
       Logger::init();
    }

    void TearDown() override
    {
    }
};

TEST_F(UtilTest, util01_base64_encode)
{
    std::string image_file = "images/mona.jpg";
    std::ifstream fin { image_file, std::ifstream::binary };
    EXPECT_FALSE(!fin);
    fin.seekg(0, fin.end);
    std::size_t length = static_cast<std::size_t>(fin.tellg());
    fin.seekg(0, fin.beg);
    std::vector<char> image_data(length);
    fin.read(image_data.data(), length);

    auto encoded_size = base64::encoded_size(length);
    std::vector<char> buffer(encoded_size);
    std::cout << "encoded_size: " << encoded_size << std::endl;

    auto result = base64::encode(buffer.data(), image_data.data(), length);
    EXPECT_TRUE(result > 0);
    std::string encoded_str(buffer.begin(), buffer.end());
    std::cout << encoded_str << std::endl;
    std::cout << "result: " << result << std::endl;


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

    auto encoded_str = aif::fileToStr("images/mona_base64.txt");
    std::cout << encoded_str << std::endl;

    auto decoded_size = base64::decoded_size(encoded_str.size());
    std::cout << "decoded_size: " << decoded_size << std::endl;

    std::vector<char> buffer(decoded_size);
    auto result = base64::decode(buffer.data(), encoded_str.data(), encoded_str.size());

    std::cout << "out.len: " << result.first << std::endl;
    std::cout << "input.len: " << result.second << std::endl;
    EXPECT_LE(result.first, buffer.size());

    // std::ofstream imgFile("images/mona_decoded.jpg", std::ios::out| std::ios::binary);
    // EXPECT_FALSE(!imgFile);
    // imgFile.write(buffer.data(), result.first);
    // imgFile.close();

    EXPECT_TRUE(aif::bufferToFile(buffer, result.first, "images/mona_decoded.jpg"));
}

TEST_F(UtilTest, util03_base64Encode_to_string)
{
    std::string base64str = aif::base64Encode("images/mona.jpg");
    EXPECT_TRUE(base64str.size() > 0);
    std::cout << base64str << std::endl;
}

TEST_F(UtilTest, util04_base64Encode_to_file)
{
    bool result = aif::base64Encode("images/mona.jpg", "images/mona_base64.txt");
    EXPECT_TRUE(result);
}

TEST_F(UtilTest, util05_base64Decode_to_buffer)
{
    std::string base64str = aif::base64Encode("images/mona.jpg");
    EXPECT_TRUE(base64str.size() > 0);
    //std::cout << base64str << std::endl;

    int dataSize = 0;
    auto data = aif::base64Decode(base64str, dataSize);
    EXPECT_TRUE(dataSize> 0);
    EXPECT_GE(data.size(), dataSize);
    Logd("data.size(): ", data.size(), " vs. dataSize: ", dataSize);
}

TEST_F(UtilTest, util06_base64Decode_to_file)
{
    std::array<std::pair<std::string, std::string>, 4> filenames{{
        { "images/mona.jpg", "images/mona_decoded.jpg" },
        { "images/bts.jpg", "images/bts_decoded.jpg" },
        { "images/blackpink.jpg", "images/blackpink_decoded.jpg" },
        { "images/doctors.jpg", "images/doctos_decoded.jpg" }
    }};

    for (const auto& f : filenames) {

        std::string base64str = aif::base64Encode(f.first);
        EXPECT_TRUE(base64str.size() > 0);

        //std::cout << base64str << std::endl;

        bool result = aif::base64Decode(base64str, f.second);
        EXPECT_TRUE(result);
    }
}

TEST_F(UtilTest, util07_base64Decode_normalize)
{
    auto base64str = aif::fileToStr("images/mona_base64.txt"); // 128 x 128

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