/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_STOPWATCH_H
#define AIF_STOPWATCH_H

#include <chrono>

namespace aif {

class Stopwatch
{
public:
    // tick type
    typedef double tick_t;

    Stopwatch();

    //	start the timer
    void start();

    //	returns true if the timer is running
    bool isStarted() const;

    //  clears the timer
    void stop();

    //	get the number of milliseconds since the timer was started
    tick_t getMs();

    void sleep(unsigned ms);

private:
    std::chrono::high_resolution_clock::time_point m_start;
};

} // end of namespace aif

#endif // AIF_STOPWATCH_H