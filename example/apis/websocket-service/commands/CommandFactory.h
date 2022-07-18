#ifndef AIF_COMMAND_FACTORY_H
#define AIF_COMMAND_FACTORY_H

#include "Command.h"

using CommandGenerator = std::shared_ptr<Command> (*)(aif::EdgeAIVision &ai, const std::string &name);

class CommandFactory
{
public:
    static CommandFactory &get()
    {
        static CommandFactory instance;
        return instance;
    }
    bool registerGenerator(
        const std::string &id,
        const CommandGenerator &generator);
    std::shared_ptr<Command> create(aif::EdgeAIVision &ai, const std::string &name);

private:
    CommandFactory() {}
    CommandFactory(const CommandFactory &) {}
    ~CommandFactory() {}

private:
    std::unordered_map<std::string, std::shared_ptr<Command>> m_commands;
    std::unordered_map<std::string, CommandGenerator> m_generators;
};

#endif // AIF_COMMAND_FACTORY_H
