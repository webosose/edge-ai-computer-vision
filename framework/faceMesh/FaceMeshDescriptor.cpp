/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/faceMesh/FaceMeshDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace rj = rapidjson;

namespace aif {

FaceMeshDescriptor::FaceMeshDescriptor()
    : Descriptor()
{
}

FaceMeshDescriptor::~FaceMeshDescriptor()
{
}

void FaceMeshDescriptor::addLandmarks(const std::vector<cv::Vec3f> &points, double score)
{
    rj::Document::AllocatorType &allocator = m_root.GetAllocator();
    if (!m_root.HasMember("facemesh"))
    {
        rj::Value facemesh(rj::kArrayType);
        m_root.AddMember("facemesh", facemesh, allocator);
    }

    rj::Value data(rj::kObjectType);

    // add landmarks: [x, y, z]
    rj::Value landmarks(rj::kArrayType);
    for (int i = 0; i < points.size(); i++)
    {
        landmarks.PushBack(points[i][0], allocator)
                 .PushBack(points[i][1], allocator)
                 .PushBack(points[i][2], allocator);
    }
    data.AddMember("landmarks", landmarks, allocator);

    // add score
    data.AddMember("score", score, allocator);

    // add face oval
    rj::Value oval(rj::kArrayType);
    auto ovalSize = sizeof(FACEMESH_OVAL) / sizeof(FACEMESH_OVAL[0]);
    for (auto i = 0; i < ovalSize; i++)
    {
        int ovalIdx = FACEMESH_OVAL[i];
        oval.PushBack(points[ovalIdx][0], allocator)
            .PushBack(points[ovalIdx][1], allocator);
    }
    data.AddMember("oval", oval, allocator);

    // add left eye
    rj::Value leftEye(rj::kArrayType);
    auto leftEyeSize = sizeof(FACEMESH_LEFTEYE) / sizeof(FACEMESH_LEFTEYE[0]);
    for (auto i = 0; i < leftEyeSize; i++)
    {
        int leftEyeIdx = FACEMESH_LEFTEYE[i];
        leftEye.PushBack(points[leftEyeIdx][0], allocator)
            .PushBack(points[leftEyeIdx][1], allocator);
    }
    data.AddMember("leftEye", leftEye, allocator);

    // add right eye
    rj::Value rightEye(rj::kArrayType);
    auto rightEyeSize = sizeof(FACEMESH_RIGHTEYE) / sizeof(FACEMESH_RIGHTEYE[0]);
    for (auto i = 0; i < rightEyeSize; i++)
    {
        int rightEyeIdx = FACEMESH_RIGHTEYE[i];
        rightEye.PushBack(points[rightEyeIdx][0], allocator)
            .PushBack(points[rightEyeIdx][1], allocator);
    }
    data.AddMember("rightEye", rightEye, allocator);

    // add mouth
    rj::Value mouth(rj::kArrayType);
    auto mouthSize = sizeof(FACEMESH_MOUTH) / sizeof(FACEMESH_MOUTH[0]);
    for (auto i = 0; i < mouthSize; i++)
    {
        int mouthIdx = FACEMESH_MOUTH[i];
        mouth.PushBack(points[mouthIdx][0], allocator)
            .PushBack(points[mouthIdx][1], allocator);
    }
    data.AddMember("mouth", mouth, allocator);

    m_root["facemesh"].PushBack(data, allocator);
}

void FaceMeshDescriptor::clear()
{
    rj::Document::AllocatorType &allocator = m_root.GetAllocator();
    m_root.RemoveMember("facemesh");
    rj::Value facemesh(rj::kArrayType);
    m_root.AddMember("facemesh", facemesh, allocator);
}

} // end of namespace aif
