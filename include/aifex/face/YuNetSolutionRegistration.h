/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YUNET_SOLUTION_REGISTRATION_H
#define AIF_YUNET_SOLUTION_REGISTRATION_H

#include <aif/base/SolutionFactoryRegistrations.h>

namespace aif {

class FaceSolution : public EdgeAISolution {
    private:
        FaceSolution() {}
        FaceSolution(const std::string& name) {}
    public:
        template <typename T>
            friend class SolutionFactoryRegistration;
        virtual ~FaceSolution() {}
        virtual std::string makeSolutionConfig(SolutionConfig config) override;
};
SolutionFactoryRegistration<FaceSolution> face_solution("face_yunet");

} // end of namespace aif

#endif // AIF_YUNET_SOLUTION_REGISTRATION_H
