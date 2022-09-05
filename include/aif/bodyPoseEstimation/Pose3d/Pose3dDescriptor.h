/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POSE3D_DESCRIPTOR_H
#define AIF_POSE3D_DESCRIPTOR_H

#include <aif/base/Descriptor.h>
#include <aif/base/Types.h>

#include <vector>

namespace aif {

enum
{
    RESULT_3D_JOINT = 0,
    RESULT_3D_TRAJ,
    RESULT_3D_IDX_MAX
};

struct Joint2D
{
    float x = 0.0f;
    float y = 0.0f;
};

struct Joint3D
{
    float x;
    float y;
    float z;
    Joint3D() : x(0.0f), y(0.0f), z(0.0f) {}
    Joint3D(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

class Pose3dDescriptor: public Descriptor
{
public:
public:
    Pose3dDescriptor();
    virtual ~Pose3dDescriptor();

    // TODO: implment add result
    void addJoints3D(const std::vector<Joint3D> &joints3d,
                     const Joint3D &joint3dPos);
//    void addJoint3DPos(const Joint3D &joint3dPos);

};

} // end of namespace aif

#endif // AIF_POSE3D_DESCRIPTOR_H
