/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "CommandFactory.h"

#include <aif/log/Logger.h>

bool CommandFactory::registerGenerator(
    const std::string &id,
    const CommandGenerator &generator)
{
    try
    {
        if (m_generators.find(id) != m_generators.end())
        {
            throw std::runtime_error(id + " generator is already registered");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << __func__ << " Error: " << e.what() << std::endl;
        return false;
    }
    catch (...)
    {
        std::cerr << __func__ << " Error: Unknown exception occured!!\n";
        return false;
    }

    m_generators[id] = generator;
    return true;
}

std::shared_ptr<Command> CommandFactory::create(aif::EdgeAIVision &ai, const std::string &name)
{
    try
    {
        if (m_commands.find(name) == m_commands.end() &&
            m_generators.find(name) == m_generators.end())
        {
            throw std::runtime_error(name + " command generator is not registered");
        }
        if (m_commands.find(name) != m_commands.end())
        {
            return m_commands[name];
        }
        m_commands[name] = m_generators[name](ai, name);
        return m_commands[name];
    }
    catch (const std::exception &e)
    {
        Loge(__func__, "Error: ", e.what());
        return nullptr;
    }
    catch (...)
    {
        Loge(__func__, "Error: Unknown exception occured!!");
        return nullptr;
    }
}