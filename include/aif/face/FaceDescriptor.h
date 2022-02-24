#ifndef AIF_FACE_DESCRIPTOR_H
#define AIF_FACE_DESCRIPTOR_H

#include <aif/base/Descriptor.h>
#include <aif/base/Types.h>
// #define RAPIDJSON_HAS_STDSTRING 1
// #include <rapidjson/document.h>

namespace aif {

class FaceDescriptor : public Descriptor
{
public:
    FaceDescriptor();
    virtual ~FaceDescriptor();

    void addFace(
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
    );

    size_t size() const { return m_faceCount; }
    void clear();

private:
    size_t m_faceCount;
};

} // end of namespace aif

#endif // AIF_FACE_DESCRIPTOR_H
