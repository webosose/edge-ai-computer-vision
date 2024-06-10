/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_COMMAND_FACTORY_REGISTRATION_H
#define AIF_COMMAND_FACTORY_REGISTRATION_H

#include "CommandFactory.h"
#include <string>

template <typename T>
class CommandFactoryRegistration
{
public:
    CommandFactoryRegistration(const std::string &name)
    {
        CommandFactory::get().registerGenerator(
            name,
            [](aif::EdgeAIVision &ai, const std::string &cmdName)
            {
                std::shared_ptr<Command> Command(new T(ai, cmdName));
                return Command;
            });
    }
};

#endif // AIF_COMMAND_FACTORY_REGISTRATION_H
