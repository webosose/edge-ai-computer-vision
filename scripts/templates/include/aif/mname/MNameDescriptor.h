#ifndef AIF_MNAME_DESCRIPTOR_H
#define AIF_MNAME_DESCRIPTOR_H

#include <aif/base/Descriptor.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

namespace aif {

class MNameDescriptor: public Descriptor
{
public:
    MNameDescriptor();
    virtual ~MNameDescriptor();

};

} // end of namespace aif

#endif // AIF_MNAME_DESCRIPTOR_H
