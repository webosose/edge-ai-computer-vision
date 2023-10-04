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
#include <aif/log/Logger.h>

namespace aif {

inline int32_t CHECK_INT_ADD(int32_t a, int32_t b) {
    int32_t sum = 0;
    if (((b > 0) && (a > (INT_MAX - b))) ||
        ((b < 0) && (a < (INT_MIN - b)))) {
        Loge("int32_t addition may overflow");
    } else {
        sum = a + b;
    }
    return sum;
}

inline int32_t CHECK_INT_SUB(int32_t a, int32_t b) {
    int32_t diff = 0;
    if ((b > 0 && a < INT_MIN + b) ||
        (b < 0 && a > INT_MAX + b)) {
        Loge("int32_t subtraction may overflow");
    } else {
        diff = a - b;
    }
    return diff;
}

inline int32_t CHECK_INT_MUL(int32_t a, int32_t b) {
    int32_t result = 0;
    if (a > 0) {  /* a is positive */
        if (b > 0) {  /* a and b are positive */
            if (a > (INT_MAX / b)) {
                Loge("int32_t multiplication may overflow");
            }
        } else { /* a positive, b nonpositive */
            if (b < (INT_MIN / a)) {
                Loge("int32_t multiplication may overflow");
            }
        } /* a positive, b nonpositive */
    } else { /* a is nonpositive */
        if (b > 0) { /* a is nonpositive, b is positive */
            if (a < (INT_MIN / b)) {
                Loge("int32_t multiplication may overflow");
            }
        } else { /* a and b are nonpositive */
            if ( (a != 0) && (b < (INT_MAX / a))) {
                Loge("int32_t multiplication may overflow");
            }
        } /* End if a and b are nonpositive */
    } /* End if a is nonpositive */

    result = a * b;
    return result;
}

inline uint32_t CHECK_UINT_ADD(uint32_t a, uint32_t b) {
    uint32_t usum = 0;
    if (UINT_MAX - a < b) {
        Loge("uint32_t addition may wrap");
    } else {
        usum = a + b;
    }
    return usum;
}

inline uint32_t CHECK_UINT_SUB(uint32_t a, uint32_t b) {
    uint32_t udiff = 0;
    if (a < b) {
        Loge("uint32_t subtraction may wrap");
    } else {
        udiff = a - b;
    }
    return udiff;
}

inline uint32_t CHECK_UINT_MUL(uint32_t a, uint32_t b) {
    uint32_t result = 0;
    if (b != 0 && a > UINT_MAX / b) {
        Loge("uint32_t multiplication may wrap");
    } else {
        result = a * b;
    }
    return result;
}

inline unsigned long CHECK_ULONG_ADD(unsigned long a, unsigned long b) {
    unsigned long usum = 0;
    if (ULONG_MAX - a < b) {
        Loge("unsigned long addition may wrap");
    } else {
        usum = a + b;
    }
    return usum;
}

inline unsigned long CHECK_ULONG_SUB(unsigned long a, unsigned long b) {
    unsigned long udiff = 0;
    if (a < b) {
        Loge("unsigned long subtraction may wrap");
    } else {
        udiff = a - b;
    }
    return udiff;
}

inline unsigned long CHECK_ULONG_MUL(unsigned long a, unsigned long b) {
    unsigned long result = 0;
    if (b != 0 && a > ULONG_MAX / b) {
        Loge("unsigned long multiplication may wrap");
    } else {
        result = a * b;
    }
    return result;
}

inline size_t INT_TO_ULONG(int value) {
    size_t ulvalue = 0;
    if (value >= 0)
        ulvalue = value;
    else
        Loge("value may result in lost or misinterpreted data");
    return ulvalue;
}

inline uint32_t INT_TO_UINT(int value) {
    uint32_t uvalue = 0;
    if (value >= 0)
        uvalue = value;
    else
        Loge("value may result in lost or misinterpreted data");
    return uvalue;
}

inline int ULONG_TO_INT(size_t value) {
    int intValue = 0;
    if (value <= INT_MAX)
        intValue = value;
    else
        Loge("value may result in lost or misinterpreted data");
    return intValue;
}

inline int LLONG_TO_INT(long long value) {
    int intValue = 0;
    if (INT_MIN <= value && value <= INT_MAX)
        intValue = value;
    else
        Loge("value may result in lost or misinterpreted data");
    return intValue;
}

inline size_t LONG_TO_ULONG(long value) {
    size_t ulvalue = 0;
    if (value >= 0)
       ulvalue = value;
    else
        Loge("value may result in lost or misinterpreted data");
    return ulvalue;
}
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
