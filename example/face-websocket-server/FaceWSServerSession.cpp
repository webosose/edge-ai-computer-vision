#include "FaceWSServerSession.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <aif/base/DetectorFactory.h>
#include <aif/tools/Stopwatch.h>
#include <aif/log/Logger.h>

using namespace aif;
namespace rj = rapidjson;

//--------------------------------------------------------------
// FaceWSServerSession class definition
//--------------------------------------------------------------

FaceWSServerSession::FaceWSServerSession(tcp::socket&& socket)
    : WSServerSession(std::move(socket))
{
}

FaceWSServerSession::~FaceWSServerSession()/* override*/
{
}

void FaceWSServerSession::onInit()/* override*/
{
//    Stopwatch sw;
//    for (auto& detector : m_detectors) {
//        sw.start();
//        detector.second->init();
//        TRACE("[Session]Detector.init(): ", detector.first, " ", sw.getMs(), "ms");
//        sw.stop();
//    }
}

void FaceWSServerSession::onHandleMessage(const std::string& message)/* override*/
{
    std::string requestName;
    try {
        rj::Document payload;
        payload.Parse(message.c_str());

        requestName = payload["req"].GetString();
        std::string requestId = payload["id"].GetString();
        std::string model = payload["model"].GetString();
        std::string base64Image = payload["arg"]["input"].GetString();

        Stopwatch sw;
        int base64ImageSize = base64Image.size();
        TRACE("base64ImageSize is ", base64ImageSize);
        if (base64ImageSize == 0) {
            throw std::runtime_error("invalid base64Image: size = 0");
        }

        std::shared_ptr<Descriptor> descriptor = DetectorFactory::get().getDescriptor(model);
        if (descriptor == nullptr) {
            throw std::runtime_error(std::string("createDescriptor failed: ") + model);
        }

        std::shared_ptr<Detector> detector = DetectorFactory::get().getDetector(model);
        if (detector == nullptr) {
            throw std::runtime_error(std::string("getDetector failed: ") + model);
        }

        sw.start();
        detector->detectFromBase64(base64Image, descriptor);
        TRACE("DETECT TIME: ", sw.getMs(), " ms");

        TRACE("request id: ", requestId);
        descriptor->addResponseName(requestName, requestId);
        descriptor->addReturnCode(kAifOk);
        response(descriptor->toStr());

    } catch(beast::system_error const& se) {
        if(se.code() == websocket::error::closed) {
            Logi(__func__, "[", se.code() , "] ", se.code().message());
        } else {
            Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        }
        onHandleError(requestName, se.code().message());
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        onHandleError(requestName, e.what());
    }
}

void FaceWSServerSession::onHandleError(
    const std::string& req, const std::string& errmsg)
{
    // { returnCode: 1, error: "xxx" }
    rj::Document res;
    res.SetObject();
    res.AddMember("returnCode", kAifError, res.GetAllocator());
    res.AddMember("req", req, res.GetAllocator());
    res.AddMember("error", errmsg, res.GetAllocator());

    rj::StringBuffer buffer;
    rj::Writer<rj::StringBuffer> writer(buffer);
    res.Accept(writer);

    response(buffer.GetString());
}
