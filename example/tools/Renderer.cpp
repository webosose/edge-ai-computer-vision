/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Bbox.h>
#include <Renderer.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

using joint_pair = std::pair<int, int>;

namespace aif {

cv::Mat Renderer::drawPose2d(const cv::Mat &img, const std::vector<std::vector<float>>& keyPoints, bool drawAll)
{
    std::map<std::string, std::vector<int>> colors;
    std::map<std::string, std::vector<std::pair<int, int>>> joints_relation_map;
    std::vector<std::string> keys;

    float threshold = 0.1f;
    if (drawAll) {
        std::vector<joint_pair> head      = { { 9, 10 }, { 9, 35 }, { 9, 36 }, { 35, 37 }, { 36, 38 } };
        joints_relation_map["head"]       = head;
        std::vector<joint_pair> neck      = { { 8, 9 } };
        joints_relation_map["neck"]       = neck;
        std::vector<joint_pair> spine     = { { 0, 7 }, { 7, 8 } };
        joints_relation_map["spine"]      = spine;
        std::vector<joint_pair> back_arch = { { 17, 18 }, { 18, 19 }, { 19, 20 }, { 17, 0 } };
        joints_relation_map["back_arch"]  = back_arch;
        std::vector<joint_pair> r_hip     = { { 0, 4 } };
        joints_relation_map["r_hip"]      = r_hip;
        std::vector<joint_pair> l_hip     = { { 0, 1 } };
        joints_relation_map["l_hip"]      = l_hip;
        std::vector<joint_pair> r_leg     = { { 4, 5 }, { 5, 6 } };
        joints_relation_map["r_leg"]      = r_leg;
        std::vector<joint_pair> l_leg     = { { 1, 2 }, { 2, 3 } };
        joints_relation_map["l_leg"]      = l_leg;
        std::vector<joint_pair> r_foot    = { { 6, 34 }, { 6, 33 } };
        joints_relation_map["r_foot"]     = r_foot;
        std::vector<joint_pair> l_foot    = { { 3, 31 }, { 3, 32 } };
        joints_relation_map["l_foot"]     = l_foot;
        std::vector<joint_pair> r_arm     = { { 8, 11 }, { 11, 12 }, { 12, 13 }, { 13, 40 } };
        joints_relation_map["r_arm"]      = r_arm;
        std::vector<joint_pair> l_arm     = { { 8, 14 }, { 14, 15 }, { 15, 16 }, { 16, 39 } };
        joints_relation_map["l_arm"]      = l_arm;
        std::vector<joint_pair> r_hand = { { 40, 26 }, { 40, 27 }, { 40, 28 }, { 40, 29 }, { 40, 30 } };
        joints_relation_map["r_hand"]  = r_hand;
        std::vector<joint_pair> l_hand = { { 39, 21 }, { 39, 22 }, { 39, 23 }, { 39, 24 }, { 39, 25 } };
        joints_relation_map["l_hand"]  = l_hand;

        std::vector<int> head_colors{ 255, 255, 255 };
        colors["head"] = head_colors;
        std::vector<int> neck_colors{ 252, 141, 243 };
        colors["neck"] = neck_colors;
        std::vector<int> spine_colors{ 252, 141, 243 };
        colors["spine"] = spine_colors;
        std::vector<int> back_arch_colors{ 252, 243, 141 };
        colors["back_arch"] = back_arch_colors;
        std::vector<int> r_hip_colors{ 56, 255, 66 };
        colors["r_hip"] = r_hip_colors;
        std::vector<int> l_hip_colors{ 66, 255, 245 };
        colors["l_hip"] = l_hip_colors;
        std::vector<int> r_leg_colors{ 102, 201, 66 };
        colors["r_leg"] = r_leg_colors;
        std::vector<int> l_leg_colors{ 44, 88, 209 };
        colors["l_leg"] = l_leg_colors;
        std::vector<int> r_foot_colors{ 255, 255, 255 };
        colors["r_foot"] = r_foot_colors;
        std::vector<int> l_foot_colors{ 255, 255, 255 };
        colors["l_foot"] = l_foot_colors;
        std::vector<int> r_arm_colors{ 60, 214, 75 };
        colors["r_arm"] = r_arm_colors;
        std::vector<int> l_arm_colors{ 38, 120, 191 };
        colors["l_arm"] = l_arm_colors;
        std::vector<int> r_hand_colors{ 255, 255, 255 };
        colors["r_hand"] = r_hand_colors;
        std::vector<int> l_hand_colors{ 255, 255, 255 };
        colors["l_hand"] = l_hand_colors;

        keys.push_back( "head" );
        keys.push_back( "neck" );
        keys.push_back( "spine" );
        keys.push_back( "back_arch" );
        keys.push_back( "r_hip" );
        keys.push_back( "l_hip" );
        keys.push_back( "r_leg" );
        keys.push_back( "l_leg" );
        keys.push_back( "r_foot" );
        keys.push_back( "l_foot" );
        keys.push_back( "r_arm" );
        keys.push_back( "l_arm" );
        keys.push_back( "r_hand" );
        keys.push_back( "l_hand" );

    } else {
        // visualizePred.py
        std::vector<joint_pair> spine     = { { 0, 7 }, { 7, 8 } };
        joints_relation_map["spine"]      = spine;
        std::vector<joint_pair> r_hip     = { { 0, 4 } };
        joints_relation_map["r_hip"]      = r_hip;
        std::vector<joint_pair> l_hip     = { { 0, 1 } };
        joints_relation_map["l_hip"]      = l_hip;
        std::vector<joint_pair> r_leg     = { { 4, 5 }, { 5, 6 } };
        joints_relation_map["r_leg"]      = r_leg;
        std::vector<joint_pair> l_leg     = { { 1, 2 }, { 2, 3 } };
        joints_relation_map["l_leg"]      = l_leg;
        std::vector<joint_pair> r_arm     = { { 8, 11 }, { 11, 12 }, { 12, 13 }, { 13, 40 } };
        joints_relation_map["r_arm"]      = r_arm;
        std::vector<joint_pair> l_arm     = { { 8, 14 }, { 14, 15 }, { 15, 16 }, { 16, 39 } };
        joints_relation_map["l_arm"]      = l_arm;

        // std::vector<int> spine_colors{ 203, 192, 255 }; // pink
        std::vector<int> spine_colors{ 255, 192, 203 }; // pink
        colors["spine"] = spine_colors;
        std::vector<int> r_hip_colors{ 0, 255, 0 }; // green
        colors["r_hip"] = r_hip_colors;
        // std::vector<int> l_hip_colors{ 255, 255, 0 }; // cyan
        std::vector<int> l_hip_colors{ 0, 255, 255 }; // cyan
        colors["l_hip"] = l_hip_colors;
        std::vector<int> r_leg_colors{ 0, 128, 0 }; // darkgreen
        colors["r_leg"] = r_leg_colors;
        // std::vector<int> l_leg_colors{ 255, 0, 0 }; // blue
        std::vector<int> l_leg_colors{ 0, 0, 255 }; // blue
        colors["l_leg"] = l_leg_colors;
        std::vector<int> r_arm_colors{ 144, 238, 144 }; // lightgreen
        colors["r_arm"] = r_arm_colors;
        // std::vector<int> l_arm_colors{ 225, 105, 65 }; // lightblue
        std::vector<int> l_arm_colors{ 64, 105, 225 }; // lightblue
        colors["l_arm"] = l_arm_colors;

        keys.push_back( "spine" );
        keys.push_back( "r_hip" );
        keys.push_back( "l_hip" );
        keys.push_back( "r_leg" );
        keys.push_back( "l_leg" );
        keys.push_back( "r_arm" );
        keys.push_back( "l_arm" );

        threshold = 0.5;
    }

    cv::Mat temp = img;
    int line_width = 2;
    // iterate over keys
    for ( unsigned int i = 0; i < keys.size(); i++ )
    {
        std::vector<joint_pair> relation = joints_relation_map[keys[i]];
        for ( unsigned int j = 0; j < relation.size(); j++ )
        {
            std::pair<int, int> relation_pair = relation[j];
            if ( keyPoints[relation_pair.first][0] > threshold && keyPoints[relation_pair.second][0] > threshold )

            {
                // Draw Line
                cv::line( temp,
                    cv::Point( int( keyPoints[relation_pair.first][1] ), int( keyPoints[relation_pair.first][2] ) ),
                    cv::Point( int( keyPoints[relation_pair.second][1] ), int( keyPoints[relation_pair.second][2] ) ),
                    cv::Scalar( colors[keys[i]][2], colors[keys[i]][1], colors[keys[i]][0] ),
                    line_width );

                // Draw Dots
                cv::circle( temp, cv::Point( int( keyPoints[relation_pair.first][1] ), int( keyPoints[relation_pair.first][2] ) ), line_width,
                            cv::Scalar( 0, 0, 255 ), 2 );
                cv::circle( temp, cv::Point( int( keyPoints[relation_pair.second][1] ), int( keyPoints[relation_pair.second][2] ) ), line_width,
                            cv::Scalar( 0, 0, 255 ), 2 );
            }
        }
    }


    // for ( unsigned int i = 0; i < keyPoints.size(); i++ )
    // {
    //     {
    //         cv::circle( temp, cv::Point( int( keyPoints[i][1] ), int( keyPoints[i][2] ) ), line_width,
    //             cv::Scalar( 255, 255, 255 ), -1 );
    //     }
    // }
    return temp;
}

cv::Mat Renderer::drawRects(
        const cv::Mat &img,
        const std::vector<cv::Rect>& rects,
        cv::Scalar color,
        int thickness)
{
    cv::Mat temp = img;
    for (const auto& rect : rects) {
        cv::rectangle(img, rect, color, thickness);
    }
    return temp;
}

cv::Mat Renderer::drawBoxes(
        const cv::Mat &img,
        const std::vector<BBox>& boxes,
        cv::Scalar color,
        int thickness)
{
    cv::Mat temp = img;
    for (const auto& box: boxes) {
        cv::Rect rect(box.xmin, box.ymin, box.width, box.height);
        cv::rectangle(img, rect, color, thickness);
    }
    return temp;
}

cv::Mat Renderer::drawRotatedRects(
            const cv::Mat &img,
            const std::vector<std::vector<cv::Point>>& rects,
            cv::Scalar color,
            int thickness)
{
    cv::Mat temp = img;
    for (const auto& rect : rects) {
        if (rect.size() < 4) continue;
        for (int i = 0; i < 4; i++) {
            cv::line(img, rect[i], rect[(i+1)%4], color, thickness);
        }
    }
    return temp;
}

}
