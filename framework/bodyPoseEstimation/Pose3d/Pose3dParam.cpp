/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/Pose3d/Pose3dParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

Pose3dParam::Pose3dParam()
    : maxBatchSize(4)
    , flipPoses(true)
    , arch{3, 3, 3} // don't need?
    , flipPoseMap{0,
                  4, 5, 6, 1, 2, 3,
                  7, 8, 9, 10,
                  14, 15, 16, 11, 12, 13,
                  17, 18, 19, 20,
                  26, 27, 28, 29, 30,
                  21, 22, 23, 24, 25,
                  33, 34, 31, 32,
                  36, 35,
                  38, 37,
                  40, 39}    // L/R flip index
    , preprocessingType(PreprocessingType::HOMOGENEOUS_COORDINATES)
    , focalLength{1469.2684222875848f, 1469.8823503910792f}
    , center{650.1274669098675f, 505.44118528424053f}
    , radialDistortion{-0.12686622768237987f, -0.33232941922945763f, 0.3877008223664106f}
    , tanDistortion{0.0015259532472412637f, 0.0029597555802232116f}
{
}

Pose3dParam::~Pose3dParam()
{
}

Pose3dParam::Pose3dParam(const Pose3dParam& other)
    : maxBatchSize(other.maxBatchSize)
    , flipPoses(other.flipPoses)
    , arch(other.arch)
    , flipPoseMap(other.flipPoseMap)
    , preprocessingType(other.preprocessingType)
    , focalLength(other.focalLength)
    , center(other.center)
    , radialDistortion(other.radialDistortion)
    , tanDistortion(other.tanDistortion)
    , pose2DJoints(other.pose2DJoints)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

Pose3dParam::Pose3dParam(Pose3dParam&& other) noexcept
    : maxBatchSize(std::move(other.maxBatchSize))
    , flipPoses(std::move(other.flipPoses))
    , arch(std::move(other.arch))
    , flipPoseMap(std::move(other.flipPoseMap))
    , preprocessingType(std::move(other.preprocessingType))
    , focalLength(std::move(other.focalLength))
    , center(std::move(other.center))
    , radialDistortion(std::move(other.radialDistortion))
    , tanDistortion(std::move(other.tanDistortion))
    , pose2DJoints(std::move(other.pose2DJoints))
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

Pose3dParam& Pose3dParam::operator=(const Pose3dParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    maxBatchSize = other.maxBatchSize;
    flipPoses = other.flipPoses;
    arch = other.arch;
    flipPoseMap = other.flipPoseMap;
    preprocessingType = other.preprocessingType;
    focalLength = other.focalLength;
    center = other.center;
    radialDistortion = other.radialDistortion;
    tanDistortion = other.tanDistortion;
    pose2DJoints = other.pose2DJoints;

    return *this;
}

Pose3dParam& Pose3dParam::operator=(Pose3dParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    maxBatchSize = std::move(other.maxBatchSize);
    flipPoses = std::move(other.flipPoses);
    arch = std::move(other.arch);
    flipPoseMap = std::move(other.flipPoseMap);
    preprocessingType = std::move(other.preprocessingType);
    focalLength = std::move(other.focalLength);
    center = std::move(other.center);
    radialDistortion = std::move(other.radialDistortion);
    tanDistortion = std::move(other.tanDistortion);
    pose2DJoints = std::move(other.pose2DJoints);

    return *this;
}

bool Pose3dParam::operator==(const Pose3dParam& other) const
{
    return (
        (maxBatchSize == other.maxBatchSize) &&
        (flipPoses == other.flipPoses) &&
        (std::equal(arch.begin(), arch.end(), other.arch.begin())) &&
        (std::equal(flipPoseMap.begin(), flipPoseMap.end(), other.flipPoseMap.begin())) &&
        (preprocessingType == other.preprocessingType) &&
        (std::abs(focalLength[0] - other.focalLength[0]) < aif::EPSILON) &&
        (std::abs(focalLength[1] - other.focalLength[1]) < aif::EPSILON) &&
        (std::abs(center[0] - other.center[0]) < aif::EPSILON) &&
        (std::abs(center[1] - other.center[1]) < aif::EPSILON) &&
        (std::abs(radialDistortion[0] - other.radialDistortion[0]) < aif::EPSILON) &&
        (std::abs(radialDistortion[1] - other.radialDistortion[1]) < aif::EPSILON) &&
        (std::abs(radialDistortion[2] - other.radialDistortion[2]) < aif::EPSILON) &&
        (std::abs(tanDistortion[0] - other.tanDistortion[0]) < aif::EPSILON) &&
        (std::abs(tanDistortion[1] - other.tanDistortion[1]) < aif::EPSILON) &&
        (pose2DJoints == other.pose2DJoints)
    );
}

bool Pose3dParam::operator!=(const Pose3dParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const Pose3dParam& fp)
{
    os << "\n{\n";
    os << "\tmaxBatchSize: " << fp.maxBatchSize << ",\n";
    os << "\tflipPoses: " << fp.flipPoses << ",\n";
    os << "\tarch: [";
    for (int i = 0; i < fp.arch.size(); i++) {
        if (i == 0) {
            os << fp.arch[i];
        } else {
            os << ", " << fp.arch[i];
        }
    }
    os << "],\n";
    os << "\tflipPoseMap: [";
    for (int i = 0; i < fp.flipPoseMap.size(); i++) {
        if (i == 0) {
            os << fp.flipPoseMap[i];
        } else {
            os << ", " << fp.flipPoseMap[i];
        }
    }
    os << "],\n";
    os << "\tpreprocessingType: " << static_cast<int>(fp.preprocessingType) << ",\n";
    os << "\tfocalLength: " << fp.focalLength[0] << ", " << fp.focalLength[1] << ",\n";
    os << "\tcenter: " << fp.center[0] << ", " << fp.center[1] << ",\n";
    os << "\tradialDistortion: " << fp.radialDistortion[0] << ", ";
    os << fp.radialDistortion[1] << ", " << fp.radialDistortion[2] << ",\n";
    os << "\ttanDistortion: " << fp.tanDistortion[0] << ", " << fp.tanDistortion[1] << "\n";
    os << "\tpose2DJoints: [\n";
    for (int i = 0; i < fp.pose2DJoints.size() / 2;) {
        os << "\t" << fp.pose2DJoints[i++] << ", " << fp.pose2DJoints[i++] << "\n";
    }
    os << "]\n";
    os << "}";

    return os;
}

void Pose3dParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

t_aif_status Pose3dParam::fromJson(const std::string& param)
{
    t_aif_status res = BodyPoseEstimationParam::fromJson(param);
    rj::Document payload;
    payload.Parse(param.c_str());
    if (payload.HasMember("modelParam")) {
        rj::Value& modelParam = payload["modelParam"];
        if (modelParam.HasMember("maxBatchSize")) {
            maxBatchSize = modelParam["maxBatchSize"].GetInt();
        }
        if (modelParam.HasMember("flipPoses")) {
            flipPoses = modelParam["flipPoses"].GetBool();
        }
        if (modelParam.HasMember("arch")) {
            arch.clear();
            for (auto& a : modelParam["arch"].GetArray()) {
                arch.push_back(a.GetInt());
            }
        }
        if (modelParam.HasMember("flipPoseMap")) {
            flipPoseMap.clear();
            for (auto& flipPose : modelParam["flipPoseMap"].GetArray()) {
                flipPoseMap.push_back(flipPose.GetInt());
            }
        }
        if (modelParam.HasMember("preprocessingType")) {
            preprocessingType = static_cast<PreprocessingType>(modelParam["preprocessingType"].GetInt());
        }
        if (modelParam.HasMember("focalLength")) {
            focalLength.fill({});
            int i = 0;
            for (auto& focalL : modelParam["focalLength"].GetArray()) {
                focalLength[i++] = focalL.GetDouble();
            }
        }
        if (modelParam.HasMember("center")) {
            center.fill({});
            int i = 0;
            for (auto& c : modelParam["center"].GetArray()) {
                center[i++] = c.GetDouble();
            }
        }
        if (modelParam.HasMember("radialDistortion")) {
            radialDistortion.fill({});
            int i = 0;
            for (auto& dist : modelParam["radialDistortion"].GetArray()) {
                radialDistortion[i++] = dist.GetDouble();
            }
        }
        if (modelParam.HasMember("tanDistortion")) {
            tanDistortion.fill({});
            int i = 0;
            for (auto& dist : modelParam["tanDistortion"].GetArray()) {
                tanDistortion[i++] = dist.GetDouble();
            }
        }/* TODO */
        if (modelParam.HasMember("pose2DJoints")) {
            for (auto& joint : modelParam["pose2DJoints"].GetArray()) {
                pose2DJoints.push_back(joint.GetDouble()); /* x, y */
            }
        }
    }

    return res;
}
}
