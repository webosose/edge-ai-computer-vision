/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/ExtensionLoader.h>
#include <aif/facade/EdgeAIVision.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <fstream>
#include <gtest/gtest.h>

#define CORE_NAME "libedgeai-vision.so"
#define BASE_EXTION_NAME "libedgeai-vision-base.so"
#define SELF_PROCESS_MAP "/proc/self/maps"

using namespace aif;
namespace rj = rapidjson;

class ExtensionLoaderTest : public ::testing::Test
{
protected:
  ExtensionLoaderTest() {}
  ~ExtensionLoaderTest() = default;
  void SetUp() override
  {
    ai.startup();
    ASSERT_TRUE(ai.isStarted());
  }

  void TearDown() override
  {
    if (ai.isStarted())
      ai.shutdown();
    EXPECT_FALSE(ai.isStarted());
  }

  bool ExistsInMaps(const std::string &lib_name)
  {
    std::ifstream maps(SELF_PROCESS_MAP);
    std::string line;
    while (std::getline(maps, line))
    {
      if (line.find(lib_name) != std::string::npos)
      {
        return true;
      }
    }
    return false;
  }

  EdgeAIVision &ai = EdgeAIVision::getInstance();
};

// This test case is to check if the EdgeAIVision is started properly.
// The extension should not be present in the memory
// before creating the detector or pipe.
TEST_F(ExtensionLoaderTest, 01_Init)
{
  ASSERT_TRUE(ai.isStarted());
  EXPECT_TRUE(this->ExistsInMaps(CORE_NAME));
  EXPECT_FALSE(this->ExistsInMaps(BASE_EXTION_NAME));
}

// This test case is to check if the extension is loaded properly
// after the EdgeAIVision::createDetector() is called.
TEST_F(ExtensionLoaderTest, 02_DetectorCreation)
{
  ASSERT_TRUE(ai.isStarted());
  ASSERT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::FACE));
  EXPECT_TRUE(this->ExistsInMaps(CORE_NAME));
  EXPECT_TRUE(this->ExistsInMaps(BASE_EXTION_NAME));
  EXPECT_TRUE(ai.deleteDetector(EdgeAIVision::DetectorType::FACE));
}


// This test case is to check if the extension is unloaded properly
// after the EdgeAIVision::shutdown() is called.
// The extension should not be present in the memory after the shutdown.
// But dlclose() doesn't guarantee that the plugin is unloaded from process memory space.
// Hence we cannot use this test case to check if the plugin is unloaded from the memory.
#if 0
TEST_F(ExtensionLoaderTest, 04_Deinit)
{
  EXPECT_TRUE(ai.shutdown());
  EXPECT_TRUE(this->ExistsInMaps(CORE_NAME));
  EXPECT_FALSE(this->ExistsInMaps(BASE_EXTION_NAME));
}
#endif
