#include <aif/bodyPoseEstimation/transforms.h>

/* AIL onnx c++ reference code */
namespace aif
{

static inline cv::Point2f
getDir( const cv::Point2f& srcPoint, const float rotRad )
{
	cv::Point2f srcResult;
	float sn = sin( rotRad );
	float cs = cos( rotRad );

	srcResult.x = ( srcPoint.x * cs ) - ( srcPoint.y * sn );
	srcResult.y = ( srcPoint.x * sn ) + ( srcPoint.y * cs );

	return srcResult;
}

static inline cv::Point2f
get3rdPoint( const cv::Point2f& a, const cv::Point2f& b )
{
	cv::Point2f direct = a - b;
	return b + cv::Point2f( -direct.y, direct.x );
}

// inv is a flag for the direction of the transform (src -> dest / dest -> src)
cv::Mat
getAffineTransform( const cv::Point2f& center, const cv::Point2f& scale,
		const float rotation, const cv::Point2f& shift,
		const int outputWidth, const int outputHeight, const bool inv, const bool useUdp )
{
	const cv::Point2f scaleTmp = scale * 200.0f;
	const float srcW = scaleTmp.x;
	const float dstW = static_cast<float>( outputWidth );
	const float dstH = static_cast<float>( outputHeight );

	const float rotRad = ( M_PI * rotation ) / 180.f;
	cv::Point2f srcDir, dstDir;

	if ( useUdp )
	{
		srcDir   = getDir( cv::Point2f( 0, ( srcW - 1 ) * -0.5f ), rotRad );
		dstDir.x = 0;
		dstDir.y = ( dstW - 1 ) * -0.5f;
	} else
	{
		srcDir   = getDir( cv::Point2f( 0, srcW * -0.5f ), rotRad );
		dstDir.x = 0;
		dstDir.y = dstW * -0.5f;
	}

	cv::Point2f src[3];
	cv::Point2f dst[3];

	src[0].x = center.x + scaleTmp.x * shift.x;
	src[0].y = center.y + scaleTmp.y * shift.y;

	src[1].x = center.x + srcDir.x + scaleTmp.x * shift.x;
	src[1].y = center.y + srcDir.y + scaleTmp.y * shift.y;

	if ( useUdp )
	{
		dst[0].x = ( dstW - 1 ) * 0.5f;
		dst[0].y = ( dstH - 1 ) * 0.5f;

		dst[1].x = ( dstW - 1 ) * 0.5f + dstDir.x;
		dst[1].y = ( dstH - 1 ) * 0.5f + dstDir.y;
	} else
	{
		dst[0].x = dstW * 0.5f;
		dst[0].y = dstH * 0.5f;

		dst[1].x = dstW * 0.5f + dstDir.x;
		dst[1].y = dstH * 0.5f + dstDir.y;
	}
	src[2] = get3rdPoint( src[0], src[1] );
	dst[2] = get3rdPoint( dst[0], dst[1] );

	if ( inv == true )
	{
		return cv::getAffineTransform( dst, src );
	}
	else
	{
		return cv::getAffineTransform( src, dst );
	}
}

} /* namespace aif */
