/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POSE2D_DESCRIPTOR_H
#define AIF_POSE2D_DESCRIPTOR_H

#include <aif/base/Descriptor.h>
#include <aif/base/Types.h>

#include <vector>

namespace aif {

class Pose2dDescriptor: public Descriptor
{
    public:
        Pose2dDescriptor();
        virtual ~Pose2dDescriptor();

        void addKeyPoints(const std::vector<std::vector<float>>& keyPoints);
        const std::vector<std::vector<float>>& getKeyPoints() const;
        size_t getPoseCount() const { return m_poseCount; }
        int getTrackId() const { return m_trackId; }
        void setTrackId(int id) { m_trackId = id; }

        enum KeyPointOutput {
            POS_X = 0,
            POS_Y = 1,
            SCORE = 2,
            NUM_KEY_POINT_ITEMS = 3
        };

        enum KeyPointType {
            HIP = 0,
            RIGHT_HIP = 1,
            RIGHT_KNEE = 2,
            RIGHT_ANKLE = 3,
            LEFT_HIP = 4,
            LEFT_KNEE = 5,
            LEFT_ANKLE = 6,
            THORAX = 7,
            NECK = 8,
            HEAD1 = 9,
            HEAD2 = 10,
            LEFT_SHOULDER = 11,
            LEFT_ELBOW = 12,
            LEFT_WRIST = 13,
            RIGHT_SHOULDER = 14,
            RIGHT_ELBOW = 15,
            RIGHT_WRIST = 16,
            SPINE1 = 17,
            SPINE2 = 18,
            SPINE3 = 19,
            SPINE4 = 20,
            RIGHT_THUMB = 21,
            RIGHT_INDEX_FINGER = 22,
            RIGHT_MIDDLE_FINGER = 23,
            RIGHT_RING_FINGER = 24,
            RIGHT_PINKY = 25,
            LEFT_THUMB = 26,
            LEFT_INDEX_FINGER = 27,
            LEFT_MIDDLE_FINGER = 28,
            LEFT_RING_FINGER = 29,
            LEFT_PINKY = 30,
            RIGHT_BIG_TOE = 31,
            RIGHT_LITTLE_TOE = 32,
            LEFT_BIG_TOE = 33,
            LEFT_LITTLE_TOE = 34,
            RIGHT_EYE = 35,
            LEFT_EYE = 36,
            RIGHT_EAR = 37,
            LEFT_EAR = 38,
            RIGHT_CENTER_KNUCKLE = 39,
            LEFT_CENTER_KNUCKLE = 40,
            KEY_POINT_TYPES = 41
        };

    private:
        size_t m_poseCount;
        int m_trackId;
        std::vector<std::vector<float>> m_keyPoints;
};

} // end of namespace aif

#endif // AIF_POSE2D_DESCRIPTOR_H
