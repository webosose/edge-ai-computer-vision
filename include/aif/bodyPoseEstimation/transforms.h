#ifndef _TRANSFORMS_H_
#define _TRANSFORMS_H_

#include <opencv2/opencv.hpp>

namespace aif
{

cv::Mat
getAffineTransform( const cv::Point2f& center, const cv::Point2f& scale,
		const float rotation, const cv::Point2f& shift,
		const int outputWidth, const int outputHeight, const bool inv, const bool useUdp );

} /* namespace BodyPoseEstimator */

#endif /* _TRANSFORMS_H_ */
