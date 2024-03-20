#include <aif/bodyPoseEstimation/common.h>

namespace aif {

BBox::BBox()
: xmin(0)
, ymin(0)
, xmax(0)
, ymax(0)
, width(0)
, height(0)
, c_x(0)
, c_y(0)
, imgWidth(0)
, imgHeight(0)
, c0(0)
, c1(0)
{
}

BBox::BBox( float width, float height )
: xmin(0)
, ymin(0)
, xmax(0)
, ymax(0)
, width(0)
, height(0)
, c_x(0)
, c_y(0)
, imgWidth(width)
, imgHeight(height)
, c0(0)
, c1(0)
{
}

BBox::BBox( float x0, float y0, float x1, float y1, int score_, int class_)
: xmin(x0)
, ymin(y0)
, xmax(x1)
, ymax(y1)
, width(0)
, height(0)
, c_x(0)
, c_y(0)
, imgWidth(0)
, imgHeight(0)
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

BBox::BBox( float xmin_, float ymin_, float width_, float height_)
: xmin(xmin_)
, ymin(ymin_)
, xmax(0)
, ymax(0)
, width(width_)
, height(height_)
, c_x(0)
, c_y(0)
, imgWidth(0)
, imgHeight(0)
, c0(0)
, c1(0)
{
    xmax = xmin + width;
    ymax = ymin + height;
}

void
BBox::addWHcc( float width_, float height_, float cx_, float cy_ )
{
    width = width_;
    height = height_;

    c_x = cx_;
    c_y = cy_;
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
