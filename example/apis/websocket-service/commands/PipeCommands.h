/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PIPE_COMMANDS_H
#define AIF_PIPE_COMMANDS_H

#include "Command.h"
#include "CommandFactoryRegistration.h"

class CreatePipeCommand : public Command
{
public:
    CreatePipeCommand(aif::EdgeAIVision &ai, const std::string &name);
    virtual bool execute(const rj::Document &request, std::string &response) override;
};
CommandFactoryRegistration<CreatePipeCommand> cmd_createPipe("createPipe");

class DeletePipeCommand : public Command
{
public:
    DeletePipeCommand(aif::EdgeAIVision &ai, const std::string &name);
    virtual bool execute(const rj::Document &request, std::string &response) override;
};
CommandFactoryRegistration<DeletePipeCommand> cmd_deletePipe("deletePipe");

class DetectPipeFromBase64Command : public Command
{
public:
    DetectPipeFromBase64Command(aif::EdgeAIVision &ai, const std::string &name);
    virtual bool execute(const rj::Document &request, std::string &response) override;
};
CommandFactoryRegistration<DetectPipeFromBase64Command> cmd_detectPipeFromBase64("detectPipeFromBase64");

#endif // AIF_PIPE_COMMANDS_H

