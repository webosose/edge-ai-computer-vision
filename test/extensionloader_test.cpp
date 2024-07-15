
#include <aif/base/Types.h>
#include <aif/base/ExtensionLoader.h>
#include <aif/tools/ConfigReader.h>
#include <aif/log/Logger.h>

#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

using namespace aif;
namespace rj = rapidjson;

class ExtensionLoaderTest : public ::testing::Test
{
protected:
    ExtensionLoaderTest() = default;
    ~ExtensionLoaderTest() = default;

    static void SetUpTestCase()
    {
      Logger::init(LogLevel::INFO);
    }

    void SetUp() override
    {
      std::string registryPath = el.getRegistryFilePath();
      std::remove(registryPath.c_str());
    }

    void TearDown() override
    {
      el.clear();
    }
public:
    ExtensionLoader &el = ExtensionLoader::get();
};

TEST_F(ExtensionLoaderTest, 01_init_without_registryfile)
{
  EXPECT_EQ(el.init(false), kAifOk);
  EXPECT_TRUE(el.isInitialized());
  std::string registryPath = el.getRegistryFilePath();
  Logi("Registry file path: ", registryPath.c_str());
  std::ifstream registryFile(registryPath);
  Logi("Registry file should not created");
  EXPECT_FALSE(registryFile.good());
}

TEST_F(ExtensionLoaderTest, 02_init_with_registryfile)
{
  EXPECT_EQ(el.init(true), kAifOk);
  EXPECT_TRUE(el.isInitialized());
  std::string registryPath = el.getRegistryFilePath();
  Logi("Registry file path: ", registryPath.c_str());
  std::ifstream registryFile(registryPath);
  Logi("Registry file is created: ", registryFile.good() ? "true" : "false");
  EXPECT_TRUE(registryFile.good());
}

TEST_F(ExtensionLoaderTest, 03_init_with_unparsed_registryfile)
{
  std::string registryPath = el.getRegistryFilePath();
  Logi("Registry file path: ", registryPath.c_str());
  std::string abnormalJson = R"(
    {
      "key": "value",
      "key_only"
    })";
  std::ofstream ofs(registryPath);
  ofs << abnormalJson;
  ofs.close();
  Logi("Registry file is created with abnormal json: ", abnormalJson.c_str());

  EXPECT_EQ(el.init(true), kAifOk);
  Logi("After ExtensionLoader::init(true) is called.");
  Logi("A normal registry file should be created.");
  
  std::stringstream buffer;
  {
    std::ifstream ifs(registryPath);
    EXPECT_TRUE(ifs.good());
    EXPECT_TRUE(ifs.is_open());
    buffer << ifs.rdbuf();
    ifs.close();
  }
  rj::Document doc;
  Logd("Parsing json: ", buffer.str().c_str());
  doc.Parse(buffer.str().c_str());
  EXPECT_FALSE(doc.HasParseError());
}

TEST_F(ExtensionLoaderTest, 04_init_retry)
{
  std::string registryPath = el.getRegistryFilePath();
  std::string abnormalJson = R"(
    {
      "key": "value",
      "key_only"
    })";
  std::ofstream registryFile(registryPath);
  registryFile << abnormalJson;
  registryFile.close();

  el.setRetryCount(3);

  //FIXME: The hardcoded path should be fixed later.
  const std::string extensionDirectoryPath = "/usr/lib/edgeai-extensions";
  const std::vector<std::string> allowedExtensionNames = {};
  EXPECT_EQ(el.initRetry(true, extensionDirectoryPath, allowedExtensionNames, true), kAifOk);
  EXPECT_EQ(el.getRetryCount(), 0);
}
