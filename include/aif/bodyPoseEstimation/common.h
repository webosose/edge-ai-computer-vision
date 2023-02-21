#ifndef AIF_BODYPOSEESTIMATION_COMMON_H
#define AIF_BODYPOSEESTIMATION_COMMON_H

#include <ostream>
#include <opencv2/opencv.hpp>

namespace aif {
/* TODO: the position...? here? */

struct Point2D
{
    float x;
    float y;
};

struct Object
{
    cv::Rect_<float> rect;
    int label;
    float prob;
    std::vector<float> class_prob;
    std::vector<Point2D> kp_center;
};

struct Scale
{
    float x;
    float y;
    Scale() : x(0.0F), y(0.0F) {}
    Scale(const float x_, const float y_) : x(x_), y(y_) {}
};

class BBox
{
public:
    float xmin;
    float ymin;
    float xmax;
    float ymax;
    float width;
    float height;
    float c_x;
    float c_y;
    int imgWidth;
    int imgHeight;

    int c0; // score
    int c1; // class

    BBox() {}
    BBox( float width, float height );
    BBox( float x0, float y0, float x1, float y1, int score_, int class_);
    BBox( float xmin_, float ymin_, float width_, float height_);
    void addXyxy( float xmin_, float ymin_, float xmax_, float ymax_, bool clip=true );
    void addTlhw( float xmin_, float ymin_, float width_, float height_ );

    friend std::ostream& operator<<( std::ostream& os, const BBox& bbox );
};


}

#endif
