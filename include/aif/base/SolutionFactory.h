/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_SOLUTION_FACTORY_H
#define AIF_SOLUTION_FACTORY_H

#include <aif/facade/EdgeAIVision.h>
#include <aif/base/Types.h>
#include <string>

namespace aif {

using SolutionGenerator = std::shared_ptr<EdgeAISolution>(*)(const std::string&);

class SolutionFactory
{
public:
    static SolutionFactory& get() {
        static SolutionFactory instance;
        return instance;
    }
    t_aif_status registerGenerator(
            const std::string& id,
            const SolutionGenerator& SolutionGenerator);

    std::shared_ptr<EdgeAISolution> getSolution(const std::string& id);
    const std::vector<std::string> getCapableSolutions();
    void deleteSolution(const std::string& id);
    void clear();

private:
    SolutionFactory() {}
    SolutionFactory(const SolutionFactory&) {}
    ~SolutionFactory() {}

private:
    std::unordered_map<std::string, std::shared_ptr<EdgeAISolution>> m_solutions;
    std::unordered_map<std::string, SolutionGenerator> m_solutionGenerators;
};

} // end of namespace aif



#endif // AIF_SOLUTION_FACTORY_H


