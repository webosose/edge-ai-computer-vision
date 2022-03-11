#ifndef AIF_POSELANDMARK_DESCRIPTOR_H
#define AIF_POSELANDMARK_DESCRIPTOR_H

#include <aif/base/Descriptor.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

namespace aif {

class PoseLandmarkDescriptor: public Descriptor
{
public:
    PoseLandmarkDescriptor();
    virtual ~PoseLandmarkDescriptor();

    void addLandmarks(float* landmarks);
    void addMaskData(int width, int height, float* mask);
    std::vector<std::vector<cv::Rect2f>>
        makeBodyParts(std::vector<std::vector<cv::Rect2f>> prev, float iouThreshold);
public:
    /*
     * 33x5 array corresponding to (x, y, z, visibility, presence).
     * - X, Y coordinates are local to the region of
     *   interest and range from [0.0, 255.0].
     * - Z coordinate is measured in "image pixels" like
     *   the X and Y coordinates and represents the
     *   distance relative to the plane of the subject's
     *   hips, which is the origin of the Z axis. Negative
     *   values are between the hips and the camera;
     *   positive values are behind the hips. Z coordinate
     *   scale is similar with X, Y scales but has different
     *   nature as obtained not via human annotation, by
     *   fitting synthetic data (GHUM model) to the 2D
     *   annotation. Note, that Z is not metric but up to
     *   scale.
     * - Visibility is in the range of [min_float, max_float]
     *   and after user-applied sigmoid denotes the
     *   probability that a keypoint is located within the
     *   frame and not occluded by another bigger body
     *   part or another object.
     * - Presence is in the range of [min_float, max_float]
     *   and after user-applied sigmoid denotes the
     *   probability that a keypoint is located within the
     *   frame.
     */
    enum LandmarkOutput {
        COOD_X = 0,
        COOD_Y = 1,
        COOD_Z = 2,
        VISIBILITY = 3,
        PRESENCE = 4,
        NUM_LANDMARK_ITEMS = 5,
    };
    enum LandmarkType {
        NOSE = 0,
        LEFT_EYE_INNER = 1,
        LEFT_EYE = 2,
        LEFT_EYE_OUTER = 3,
        RIGHT_EYE_INNER = 4,
        RIGHT_EYE = 5,
        RIGHT_EYE_OUTER = 6,
        LEFT_EAR = 7,
        RIGHT_EAR = 8,
        MOUTH_LEFT = 9,
        MOUTH_RIGHT = 10,
        LEFT_SHOULDER = 11,
        RIGHT_SHOULDER = 12,
        LEFT_ELBOW = 13,
        RIGHT_ELBOW = 14,
        LEFT_WRIST = 15,
        RIGHT_WRIST = 16,
        LEFT_PINKY = 17,
        RIGHT_PINKY = 18,
        LEFT_INDEX = 19,
        RIGHT_INDEX = 20,
        LEFT_THUMB = 21,
        RIGHT_THUMB = 22,
        LEFT_HIP = 23,
        RIGHT_HIP = 24,
        LEFT_KNEE = 25,
        RIGHT_KNEE = 26,
        LEFT_ANKLE = 27,
        RIGHT_ANKLE = 28,
        LEFT_HEEL = 29,
        RIGHT_HEEL = 30,
        LEFT_FOOT_INDEX = 31,
        RIGHT_FOOT_INDEX = 32,
        NUM_LANDMARK_TYPES = 33
    };

protected:
    cv::Rect2f getRect(LandmarkType beignType, LandmarkType endType) const;
    float getScore(LandmarkType beginType, LandmarkType endType) const;

private:
    std::vector<int> m_mask;
    std::vector<std::vector<float>> m_landmarks;
    std::vector<std::vector<cv::Rect2f>> m_prevPoses;
};

} // end of namespace aif

#endif // AIF_POSELANDMARK_DESCRIPTOR_H
