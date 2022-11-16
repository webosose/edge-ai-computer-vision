#ifndef AIF_PERSONDETECT_DETECTOR_H
#define AIF_PERSONDETECT_DETECTOR_H

#include <aif/base/Detector.h>

namespace aif {

class PersonDetector : public Detector
{
public:
    PersonDetector(const std::string& modelPath)
                     : Detector(modelPath)
                     , mOrigImgRoiX(0)
                     , mOrigImgRoiY(0)
                     , mOrigImgRoiWidth(0)
                     , mOrigImgRoiHeight(0) {}
    virtual ~PersonDetector() {}

protected:
    bool isRoiValid( const int imgWidth, const int imgHeight ) const
    {
        return ( ( mOrigImgRoiX >= 0 ) && ( mOrigImgRoiY >= 0 ) &&
                        ( mOrigImgRoiWidth > 0 ) && ( mOrigImgRoiHeight > 0 ) &&
                        ( mOrigImgRoiX + mOrigImgRoiWidth <= imgWidth ) &&
                        ( mOrigImgRoiY + mOrigImgRoiHeight <= imgHeight ) );
    }

protected:
    int mOrigImgRoiX;
    int mOrigImgRoiY;
    int mOrigImgRoiWidth;
    int mOrigImgRoiHeight;
};

}; // end of namespace aif


#endif // AIF_PERSONDETECT_DETECTOR_H
