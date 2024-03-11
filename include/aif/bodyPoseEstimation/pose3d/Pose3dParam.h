/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POSE3D_PARAM_H
#define AIF_POSE3D_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>
#include <aif/bodyPoseEstimation/BodyPoseEstimationParam.h>

#include <ostream>
#include <array>

namespace aif {

class Pose3dParam : public BodyPoseEstimationParam
{
public:
    enum class PreprocessingType {
        NORMAL = 0,
        HOMOGENEOUS_COORDINATES = 1,
    };

    enum class AlternativeInputType {
        IGNORE = 0,
        REPLICATE = 1,
        INTERPOLATE = 2,
    };

    Pose3dParam();
    virtual ~Pose3dParam();
    Pose3dParam(const Pose3dParam& other);
    Pose3dParam(Pose3dParam&& other) noexcept;

    Pose3dParam& operator=(const Pose3dParam& other);
    Pose3dParam& operator=(Pose3dParam&& other) noexcept;

    bool operator==(const Pose3dParam& other) const;
    bool operator!=(const Pose3dParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const Pose3dParam& fp);
    void trace();

    t_aif_status fromJson(const std::string& param) override;

public:
    int maxBatchSize; // 4
    bool flipPoses;   //true
    std::vector<int> arch;               // "ARCH": "3, 3, 3",///dont need?
    std::vector<int> flipPoseMap;        // "FLIP_POSE_MAP": "0, 4, 5, 6, 1, 2, 3, 7, 8, 9, 10, 14, 15, 16, 11, 12, 13, 17, 18, 19, 20, 26, 27, 28, 29, 30, 21, 22, 23, 24, 25, 33, 34, 31, 32, 36, 35, 38, 37, 40, 39",
    PreprocessingType preprocessingType;
    AlternativeInputType alternativeInputType;

    std::array<float, 2> focalLength;    // 1469.2684222875848F, 1469.8823503910792F
    std::array<float, 2> center;         // 650.1274669098675F, 505.44118528424053F
    std::array<float, 3> radialDistortion;   // -0.12686622768237987F, -0.33232941922945763F, 0.3877008223664106F
    std::array<float, 2> tanDistortion;  // 0.0015259532472412637F, 0.0029597555802232116F

    /* TODO: array!! */
    std::vector<float> pose2DJoints;

};

} // end of namespace aif

#endif // AIF_POSE3D_PARAM_H
