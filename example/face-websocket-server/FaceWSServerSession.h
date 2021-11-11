#ifndef FACE_WSSERVER_SESSION_H
#define FACE_WSSERVER_SESSION_H

#include <aif/base/WSServerSession.h>
#include <aif/base/Detector.h>

#include <map>

class FaceWSServerSession : public aif::WSServerSession
{
public:
    FaceWSServerSession(tcp::socket&& socket);

    ~FaceWSServerSession() override;

protected:
    void onInit() override;

    void onHandleMessage(const std::string& message) override;

private:
//    std::shared_ptr<aif::Descriptor>
//    createDescriptor(const std::string& req);

//    std::shared_ptr<aif::Detector>
//    getDetector(const std::string& req, const std::string& hwAccelator);

    void onHandleError(const std::string& req, const std::string& errmsg);

//private:
//    std::unordered_map<
//        std::string,
//        std::shared_ptr<aif::Detector>
//        > m_detectors;
};

#endif // end of FACE_WSSERVER_SESSION_H
