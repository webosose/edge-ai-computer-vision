#include <aif/face/FaceDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

FaceDescriptor::FaceDescriptor()
    : Descriptor()
    , m_faceCount(0)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    rj::Value faces(rj::kArrayType);
    m_root.AddMember("faces", faces, allocator);
}

FaceDescriptor::~FaceDescriptor()
{
}

void FaceDescriptor::addFace(
        float score,
        float region_x,
        float region_y,
        float region_w,
        float region_h,
        float lefteye_x,
        float lefteye_y,
        float righteye_x,
        float righteye_y,
        float nosetip_x,
        float nosetip_y,
        float mouth_x,
        float mouth_y,
        float leftear_x,
        float leftear_y,
        float rightear_x,
        float rightear_y
    )
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("faces")) {
        rj::Value faces(rj::kArrayType);
        m_root.AddMember("faces", faces, allocator);
    }

    rj::Value face(rj::kObjectType);
    face.AddMember("score", score, allocator);

    // region: [x, y, w, h]
    rj::Value region(rj::kArrayType);
    region.PushBack(region_x, allocator)
          .PushBack(region_y, allocator)
          .PushBack(region_w, allocator)
          .PushBack(region_h, allocator);
    face.AddMember("region", region, allocator);

    // lefteye: [x, y]
    rj::Value lefteye(rj::kArrayType);
    lefteye.PushBack(lefteye_x, allocator)
           .PushBack(lefteye_y, allocator);
    face.AddMember("lefteye", lefteye, allocator);

    // righteye: [x, y]
    rj::Value righteye(rj::kArrayType);
    righteye.PushBack(righteye_x, allocator)
            .PushBack(righteye_y, allocator);
    face.AddMember("righteye", righteye, allocator);

    // nosetip: [x, y]
    rj::Value nosetip(rj::kArrayType);
    nosetip.PushBack(nosetip_x, allocator)
           .PushBack(nosetip_y, allocator);
    face.AddMember("nosetip", nosetip, allocator);

    // mouth: [x, y]
    rj::Value mouth(rj::kArrayType);
    mouth.PushBack(mouth_x, allocator)
         .PushBack(mouth_y, allocator);
    face.AddMember("mouth", mouth, allocator);

    // leftear: [x, y]
    rj::Value leftear(rj::kArrayType);
    leftear.PushBack(leftear_x, allocator)
           .PushBack(leftear_y, allocator);
    face.AddMember("leftear", leftear, allocator);

    // rightear: [x, y]
    rj::Value rightear(rj::kArrayType);
    rightear.PushBack(rightear_x, allocator)
            .PushBack(rightear_y, allocator);
    face.AddMember("rightear", rightear, allocator);

    m_root["faces"].PushBack(face, allocator);
    m_faceCount++;
}

void FaceDescriptor::clear()
{
    m_faceCount = 0;
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    m_root.RemoveMember("faces");
    rj::Value faces(rj::kArrayType);
    m_root.AddMember("faces", faces, allocator);
}

} // end of namespace aif
