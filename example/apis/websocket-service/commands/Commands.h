/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_COMMANDS_H
#define AIF_COMMANDS_H

#include "Command.h"
#include "CommandFactoryRegistration.h"

class StartupCommand : public Command
{
public:
    StartupCommand(aif::EdgeAIVision &ai, const std::string &name);
    virtual bool execute(const rj::Document &request, std::string &response) override;
};
CommandFactoryRegistration<StartupCommand> cmd_startup("startup");

class CreateDetectorCommand : public Command
{
public:
    CreateDetectorCommand(aif::EdgeAIVision &ai, const std::string &name);
    virtual bool execute(const rj::Document &request, std::string &response) override;
};
CommandFactoryRegistration<CreateDetectorCommand> cmd_createDetector("createDetector");

class DeleteDetectorCommand : public Command
{
public:
    DeleteDetectorCommand(aif::EdgeAIVision &ai, const std::string &name);
    virtual bool execute(const rj::Document &request, std::string &response) override;
};
CommandFactoryRegistration<DeleteDetectorCommand> cmd_deleteDetector("deleteDetector");

class DetectFromBase64Command : public Command
{
public:
    DetectFromBase64Command(aif::EdgeAIVision &ai, const std::string &name);
    virtual bool execute(const rj::Document &request, std::string &response) override;
};
CommandFactoryRegistration<DetectFromBase64Command> cmd_detectFromBase64("detectFromBase64");

#endif // AIF_COMMANDS_H
