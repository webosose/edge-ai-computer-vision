/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_DETECTOR_FACTORY_REGISTRATION_H
#define AIF_DETECTOR_FACTORY_REGISTRATION_H

#include <aif/base/Types.h>
#include <aif/base/Detector.h>
#include <aif/base/DetectorFactory.h>

#include <string>

namespace aif {

template <typename T1, typename T2>
class DetectorFactoryRegistration
{
public:
    DetectorFactoryRegistration(const std::string& id) {
        DetectorFactory::get().registerGenerator(
                id,
                []() {
                    std::shared_ptr<Detector> detector(new T1());
                    return detector;
                },
                []() {
                    std::shared_ptr<Descriptor> descriptor(new T2());
                    return descriptor;
                }
        );
    }
};

} // end of namespace aif

#endif  // AIF_DETECTOR_FACTORY_REGISTRATION_H
