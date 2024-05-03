/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ExtensionInspector.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <fstream>
#include <gtest/gtest.h>

#include <rapidjson/document.h>
#include <rapidjson/schema.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>

#define EXTENSION_VALID_DIR "/usr/lib/edgeai-extensions"
#define EXTENSION_INVALID_DIR "/usr/lib/foo/bar"
#define EXTENSION_INFO_FILE_PATH "/tmp/edgeai-extensions.json"
#define EXTENSION_INFO_FILE_SCHEMA "edgeai-extensions.schema"

using namespace aif;
namespace rj = rapidjson;

class ExtensionInspectorTest : public ::testing::Test
{
protected:
  ExtensionInspectorTest()
  {
    schemaJson = R"({
        "$schema": "http://json-schema.org/draft-07/schema#",
        "type": "array",
        "items": {
            "type": "object",
            "properties": {
                "name": {
                    "type": "string"
                },
                "path": {
                    "type": "string"
                },
                "alias": {
                    "type": "string"
                },
                "version": {
                    "type": "string"
                },
                "description": {
                    "type": "string"
                },
                "features": {
                    "type": "array",
                    "items": {
                        "type": "object",
                        "properties": {
                            "name": {
                                "type": "string"
                            },
                            "type": {
                                "type": "string"
                            }
                        },
                        "required": ["name", "type"]
                    }
                }
            },
            "required": ["name", "path", "features"]
        }
    })";
  };

  ~ExtensionInspectorTest() = default;
  void SetUp() override
  {
  }

  void TearDown() override
  {
  }

  bool IsValidJson(const std::string &json)
  {
    rapidjson::Document schemaDoc;
    schemaDoc.Parse(schemaJson.c_str());
    rapidjson::SchemaDocument schema = rj::SchemaDocument(schemaDoc);
    rapidjson::SchemaValidator validator(schema);
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError())
    {
      return false;
    }
    return doc.Accept(validator);
  }

  std::string schemaJson;
  ExtensionInspector& inspector = ExtensionInspector::get();
};

TEST_F(ExtensionInspectorTest, 01_ValidInspect)
{
  inspector.setPluginPath(EXTENSION_VALID_DIR);
  EXPECT_EQ(inspector.inspect(), aif::kAifOk);
}

TEST_F(ExtensionInspectorTest, 02_InvalidInspect)
{
  inspector.setPluginPath(EXTENSION_INVALID_DIR);
  EXPECT_EQ(inspector.inspect(), aif::kAifError);
}

TEST_F(ExtensionInspectorTest, 03_NotEmptyJson)
{
  inspector.setPluginPath(EXTENSION_VALID_DIR);
  EXPECT_EQ(inspector.inspect(), aif::kAifOk);
  std::string json = inspector.json();
  EXPECT_FALSE(json.empty());
}

TEST_F(ExtensionInspectorTest, 04_ValidateJson)
{
  inspector.setPluginPath(EXTENSION_VALID_DIR);
  EXPECT_EQ(inspector.inspect(), aif::kAifOk);
  std::string json = inspector.json();
  EXPECT_TRUE(this->IsValidJson(json));
}
