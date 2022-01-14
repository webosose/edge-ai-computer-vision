#ifndef AIF_PALM_DESCRIPTOR_H
#define AIF_PALM_DESCRIPTOR_H

#include <aif/base/Descriptor.h>
#include <aif/base/Types.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

namespace aif {

class PalmDescriptor: public Descriptor
{
public:
    PalmDescriptor();
    virtual ~PalmDescriptor();

    void addPalm(float score, const std::vector<float>& palmPoints);
    size_t size() const { return m_palmCount; }
    void clear();

private:
    size_t m_palmCount;
};

} // end of namespace aif

#endif // AIF_PALM_DESCRIPTOR_H
