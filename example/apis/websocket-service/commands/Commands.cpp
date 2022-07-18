#include "Commands.h"

#include <aif/tools/Utils.h>

using namespace aif;
namespace rj = rapidjson;

StartupCommand::StartupCommand(aif::EdgeAIVision &ai, const std::string &name)
    : Command(ai, name)
{
}

bool StartupCommand::execute(const rj::Document &request, std::string &response)
{
    bool res = m_ai.startup();
    m_result.addReturnCode(res ? aif::kAifOk : aif::kAifError);
    response = m_result.toStr();
    return res;
}

CreateDetectorCommand::CreateDetectorCommand(aif::EdgeAIVision &ai, const std::string &name)
    : Command(ai, name)
{
}

bool CreateDetectorCommand::execute(const rj::Document &request, std::string &response)
{
    aif::EdgeAIVision::DetectorType dtype = static_cast<EdgeAIVision::DetectorType>(request["type"].GetInt());
    std::string option = jsonObjectToString(request["option"]);
    Logi("type: ", static_cast<int>(dtype));
    Logi("option: ", option);
    bool res = m_ai.createDetector(dtype, option);
    m_result.addReturnCode(res ? aif::kAifOk : aif::kAifError);
    response = m_result.toStr();
    return res;
}

DeleteDetectorCommand::DeleteDetectorCommand(aif::EdgeAIVision &ai, const std::string &name)
    : Command(ai, name)
{
}

bool DeleteDetectorCommand::execute(const rj::Document &request, std::string &response)
{
    aif::EdgeAIVision::DetectorType dtype = static_cast<EdgeAIVision::DetectorType>(request["type"].GetInt());
    Logi("type: ", static_cast<int>(dtype));
    bool res = m_ai.deleteDetector(dtype);
    m_result.addReturnCode(res ? aif::kAifOk : aif::kAifError);
    response = m_result.toStr();
    return res;
}

DetectFromBase64Command::DetectFromBase64Command(aif::EdgeAIVision &ai, const std::string &name)
    : Command(ai, name)
{
}

bool DetectFromBase64Command::execute(const rj::Document &request, std::string &response)
{
    aif::EdgeAIVision::DetectorType dtype = static_cast<EdgeAIVision::DetectorType>(request["type"].GetInt());
    Logi("type: ", static_cast<int>(dtype));
    std::string base64Image = request["input"].GetString();
    std::string output;
    bool res = m_ai.detectFromBase64(dtype, base64Image, output);
    m_result.addReturnCode(res ? aif::kAifOk : aif::kAifError);
    response = output;
    return res;
}