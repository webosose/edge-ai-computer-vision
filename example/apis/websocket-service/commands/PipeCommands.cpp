/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "PipeCommands.h"

#include <aif/tools/Utils.h>

using namespace aif;
namespace rj = rapidjson;

CreatePipeCommand::CreatePipeCommand(aif::EdgeAIVision &ai, const std::string &name)
    : Command(ai, name)
{
}

bool CreatePipeCommand::execute(const rj::Document &request, std::string &response)
{
    std::string id = request["id"].GetString();
    std::string option = jsonObjectToString(request["option"]);
    Logi("id: ", id);
    Logi("option: ", option);
    bool res = m_ai.pipeCreate(id, option);
    m_result.addReturnCode(res ? aif::kAifOk : aif::kAifError);
    response = m_result.toStr();
    return res;
}

DeletePipeCommand::DeletePipeCommand(aif::EdgeAIVision &ai, const std::string &name)
    : Command(ai, name)
{
}

bool DeletePipeCommand::execute(const rj::Document &request, std::string &response)
{
    std::string id = request["id"].GetString();
    Logi("id: ", id);
    bool res = m_ai.pipeDelete(id);
    m_result.addReturnCode(res ? aif::kAifOk : aif::kAifError);
    response = m_result.toStr();
    return res;
}

DetectPipeFromBase64Command::DetectPipeFromBase64Command(aif::EdgeAIVision &ai, const std::string &name)
    : Command(ai, name)
{
}

bool DetectPipeFromBase64Command::execute(const rj::Document &request, std::string &response)
{
    std::string id = request["id"].GetString();
    Logi("id: ", id);
    std::string base64Image = request["input"].GetString();
    std::string output;
    bool res = m_ai.pipeDetectFromBase64(id, base64Image, output);
    m_result.addReturnCode(res ? aif::kAifOk : aif::kAifError);
    response = output;
    return res;
}
