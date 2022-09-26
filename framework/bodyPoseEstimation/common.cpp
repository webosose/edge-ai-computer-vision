#include <aif/bodyPoseEstimation/common.h>

namespace aif {

BBox::BBox( float width, float height )
{
    imgWidth = width;
    imgHeight = height;
}

BBox::BBox( float x0, float y0, float x1, float y1, int score_, int class_)
    : xmin(x0)
    , ymin(y0)
    , xmax(x1)
    , ymax(y1)
    , c0(score_)
    , c1(class_)
{
    // compute width & height
    width = xmax - xmin;
    height = ymax - ymin;

    // compute center
    c_x = xmin + round( width / 2.0f );
    c_y = ymin + round( height / 2.0f );
}

void
BBox::addXyxy( float xmin_, float ymin_, float xmax_, float ymax_, bool clip )
{
    if ( clip )
    {
        xmin = ( xmin_ < 0 ? 0 : xmin_ );
        ymin = ( ymin_ < 0 ? 0 : ymin_ );
        xmax = ( xmax_ > imgWidth - 1 ? imgWidth - 1 : xmax_ );
        ymax = ( ymax_ > imgHeight - 1 ? imgHeight - 1 : ymax_ );
    }
    else
    {
        xmin = xmin_;
        ymin = ymin_;
        xmax = xmax_;
        ymax = ymax_;
    }

    // compute width & height
    width = xmax - xmin;
    height = ymax - ymin;

    // compute center
    c_x = xmin + round( width / 2.0f );
    c_y = ymin + round( height / 2.0f );
}

void
BBox::addTlhw( float xmin_, float ymin_, float width_, float height_ )
{
        addXyxy( xmin_, ymin_, xmin_ + width_, ymin_ + height_, false );
}

std::ostream&
operator<<( std::ostream& os, const BBox& bbox )
{
        os << "Bounding box in format xyxy: " << bbox.xmin << " " << bbox.ymin << " "
                        << bbox.xmax << " " << bbox.ymax;
        return os;
}

} // end of namespace aif
