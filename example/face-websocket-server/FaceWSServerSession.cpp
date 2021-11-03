#include "FaceWSServerSession.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <aif/face/FaceDetectorFactory.h>
#include <aif/pose/PosenetDetectorFactory.h>
#include <aif/movenet/MovenetDetectorFactory.h>
#include <aif/bodypix/BodypixDetectorFactory.h>
#include <aif/semantic/SemanticDetectorFactory.h>
#include <aif/selfie/SelfieDetectorFactory.h>
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
    m_detectors["face_detect_cpu"] = FaceDetectorFactory::create("short_range");
    m_detectors["posenet_detect_cpu"] = PosenetDetectorFactory::create("posenet_cpu");
    m_detectors["movenet_detect_cpu"] = MovenetDetectorFactory::create("movenet_cpu");
    m_detectors["bodypix_detect_cpu"] = BodypixDetectorFactory::create("bodypix_cpu");
    m_detectors["semantic_detect_cpu"] = SemanticDetectorFactory::create("semantic_cpu");
    m_detectors["selfie_detect_cpu"] = SelfieDetectorFactory::create("selfie_cpu");
#ifdef USE_EDGETPU
    m_detectors["face_detect_edgetpu"] = FaceDetectorFactory::create("short_range_edgetpu");
    m_detectors["posenet_detect_edgetpu"] = PosenetDetectorFactory::create("posenet_edgetpu");
    m_detectors["movenet_detect_edgetpu"] = MovenetDetectorFactory::create("movenet_edgetpu");
    m_detectors["bodypix_detect_edgetpu"] = BodypixDetectorFactory::create("bodypix_edgetpu");
    m_detectors["semantic_detect_edgetpu"] = SemanticDetectorFactory::create("semantic_edgetpu");
#endif



}

FaceWSServerSession::~FaceWSServerSession()/* override*/
{
}

void FaceWSServerSession::onInit()/* override*/
{
    Stopwatch sw;
    for (auto& detector : m_detectors) {
        sw.start();
        detector.second->init();
        TRACE("[Session]Detector.init(): ", detector.first, " ", sw.getMs(), "ms");
        sw.stop();
    }
}

void FaceWSServerSession::onHandleMessage(const std::string& message)/* override*/
{
    std::string requestName;
    try {
        rj::Document payload;
        payload.Parse(message.c_str());

        requestName = payload["req"].GetString();
        std::string hwAccelator = payload["hw_accelator"].GetString();
        std::string base64Image = payload["arg"]["input"].GetString();
        std::string id = "none";
        if (payload["arg"].HasMember("id")) {
            id = payload["arg"]["id"].GetString();
        }

        Stopwatch sw;
        int base64ImageSize = base64Image.size();
        TRACE("base64ImageSize is ", base64ImageSize);
        if (base64ImageSize == 0) {
            throw std::runtime_error("invalid base64Image: size = 0");
        }

        std::shared_ptr<Descriptor> descriptor = createDescriptor(requestName);
        if (descriptor == nullptr) {
            throw std::runtime_error(std::string("createDescriptor failed: ")
                + requestName);
        }

        std::shared_ptr<Detector> detector = getDetector(requestName, hwAccelator);
        if (detector == nullptr) {
            throw std::runtime_error(std::string("getDetector failed: ")
                + requestName);
        }

        sw.start();
        detector->detectFromBase64(base64Image, descriptor);
        TRACE("DETECT TIME: ", sw.getMs(), " ms");

        descriptor->addResponseName(requestName, id);
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

std::shared_ptr<Descriptor>
FaceWSServerSession::createDescriptor(const std::string& req)
{
    if (req == "face_detect")
        return std::make_shared<FaceDescriptor>();
    else if (req == "posenet_detect")
        return std::make_shared<PosenetDescriptor>();
    else if (req == "movenet_detect")
        return std::make_shared<MovenetDescriptor>();
    else if (req == "bodypix_detect")
        return std::make_shared<BodypixDescriptor>();
    else if (req == "semantic_detect")
        return std::make_shared<SemanticDescriptor>();
    else if (req == "selfie_detect")
        return std::make_shared<SelfieDescriptor>();

   std::runtime_error(
        std::string("cannot create descriptor(unknown request name: ")
            + req +")");
    return nullptr;
}

std::shared_ptr<Detector>
FaceWSServerSession::getDetector(
    const std::string& req, const std::string& hwAccelator)
{
    std::string detectorName = req + "_" + hwAccelator;
    if (m_detectors.find(detectorName) == m_detectors.end()) {
        std::runtime_error(
            std::string("unknown detector name: ") + detectorName);
    }
    return m_detectors[detectorName];
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
