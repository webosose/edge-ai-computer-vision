/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/handLandmark/HandLandmarkDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

HandLandmarkDescriptor::HandLandmarkDescriptor()
    : Descriptor()
{
}

HandLandmarkDescriptor::~HandLandmarkDescriptor()
{
}

void HandLandmarkDescriptor::addLandmarks(float presence, float handedness, float* landmarks, float* landmarks3d)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("hands")) {
        rj::Value hands(rj::kArrayType);
        m_root.AddMember("hands", hands, allocator);
    }
    rj::Value hand(rj::kObjectType);
    rj::Value points(rj::kArrayType);
    for (size_t i = 0; i < LANDMARK_TYPES::LANDMARK_TYPE_COUNT; i++) {
        rj::Value point(rj::kArrayType);
        for (size_t j = i * LANDMARK_ITEM_COUNT; j < (i+1) * LANDMARK_ITEM_COUNT; j++) {
            point.PushBack(landmarks[j], allocator);
        }
        points.PushBack(point, allocator);
    }
    rj::Value points3d(rj::kArrayType);
    for (size_t i = 0; i < LANDMARK_TYPES::LANDMARK_TYPE_COUNT; i++) {
        rj::Value point(rj::kArrayType);
        for (size_t j = i * LANDMARK_ITEM_COUNT; j < (i+1) * LANDMARK_ITEM_COUNT; j++) {
            point.PushBack(landmarks3d[j], allocator);
        }
        points3d.PushBack(point, allocator);
    }
 
    hand.AddMember("presence", presence, allocator);
    hand.AddMember("handedness", handedness, allocator);
    hand.AddMember("landmarks", points, allocator);
    hand.AddMember("landmarks3d", points3d, allocator);
    m_root["hands"].PushBack(hand, allocator);
}

}
