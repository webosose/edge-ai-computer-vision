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
    Pose3dDescriptor();
    virtual ~Pose3dDescriptor();

    void addJoints3D(const std::vector<Joint3D> &joints3d);
    void addTraj3D(const Joint3D &joint3dTraj);

    int getTrackId() const { return m_trackId; }
    void setTrackId(int id) { m_trackId = id; }

    const std::vector<Joint3D>& getPose3dResult() const
    { return m_3dResult; }
    const Joint3D& getTrajectory() const
    { return m_trajectory; }

private:
    int m_trackId;
    std::vector<Joint3D> m_3dResult;
    Joint3D m_trajectory;
};

} // end of namespace aif

#endif // AIF_POSE3D_DESCRIPTOR_H
