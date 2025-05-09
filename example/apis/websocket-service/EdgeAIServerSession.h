/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FACE_WSSERVER_SESSION_H
#define FACE_WSSERVER_SESSION_H

#include <WSServerSession.h>
#include <aif/facade/EdgeAIVision.h>

#include <map>

class EdgeAIServerSession : public aif::WSServerSession
{
public:
    EdgeAIServerSession(tcp::socket &&socket);

    ~EdgeAIServerSession() override;

protected:
    void onInit() override;
    void onHandleMessage(const std::string &message) override;

private:
    void onHandleError(const std::string &req, const std::string &errmsg);

    aif::EdgeAIVision &m_ai;
};

#endif // end of FACE_WSSERVER_SESSION_H
