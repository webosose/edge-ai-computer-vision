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
    void onHandleError(const std::string& req, const std::string& errmsg);

};

#endif // end of FACE_WSSERVER_SESSION_H
