/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_TYPES_H
#define AIF_TYPES_H

#include <utility>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <string>

namespace aif {

static const float EPSILON = 0.000001f;

typedef enum {
    kAifOk = 0,
    kAifError = 1
} t_aif_status;

typedef enum {
    kAifNone = 0,
    kAifNormalization = 1,
    kAifStandardization = 2
} t_aif_rescale;

typedef enum {
    kWebSocketOpen = 0,
    kWebSocketClose = 1,
    kWebSocketMessage = 2,
    kWebSocketError = 3
} t_aif_event_type;

struct t_aif_modelinfo
{
    int inputSize;
    int batchSize;
    int width;
    int height;
    int channels;
};

struct t_aif_anchor
{
    float x_center;
    float y_center;
    float w;
    float h;
};

struct t_aif_rect
{
    float xmin;
    float ymin;
    float xmax;
    float ymax;

    t_aif_rect(float _xmin=0.f, float _ymin=0.f, float _xmax=0.f, float _ymax=0.f)
        : xmin{_xmin}, ymin{_ymin}, xmax{_xmax}, ymax{_ymax} {}

    t_aif_rect(const t_aif_rect& other)
        : xmin{other.xmin}, ymin{other.ymin}, xmax{other.xmax}, ymax{other.ymax} {}

    t_aif_rect(t_aif_rect&& other) noexcept
        : xmin{std::exchange(other.xmin, 0.f)}, ymin{std::exchange(other.ymin, 0.f)}
        , xmax{std::exchange(other.xmax, 0.f)}, ymax{std::exchange(other.ymax, 0.f)} {}

    t_aif_rect& operator=(const t_aif_rect& other) {
        if (this == &other) { return *this;  }
        xmin = other.xmin;  xmax = other.xmax;
        ymin = other.ymin;  ymax = other.ymax;
        return *this;
    }

    t_aif_rect& operator=(t_aif_rect&& other) noexcept {
        if (this == &other) { return *this; }
        xmin = std::exchange(other.xmin, 0.f);  xmax = std::exchange(other.xmax, 0.f);
        ymin = std::exchange(other.ymin, 0.f);  ymax = std::exchange(other.ymax, 0.f);
        return *this;
    }

    bool operator==(const t_aif_rect& other) const {
        return (std::abs(xmin - other.xmin) < EPSILON &&
                std::abs(xmax - other.xmax) < EPSILON &&
                std::abs(ymin - other.ymin) < EPSILON &&
                std::abs(ymax - other.ymax) < EPSILON);
    }

    bool operator!=(const t_aif_rect& other) const {
        return !operator==(other);
    }
};

struct t_aif_event
{
    t_aif_event_type type;
    std::string data;
};

} // end of namespace aif

#endif // AIF_TYPES_H
