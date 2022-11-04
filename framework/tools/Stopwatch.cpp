/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/tools/Stopwatch.h>

#include <iostream>
#include <thread>

namespace aif {

using tick_t = Stopwatch::tick_t;

Stopwatch::Stopwatch()
    : m_start(std::chrono::high_resolution_clock::time_point::min())
{ }

//	start the timer
void Stopwatch::start()
{
    m_start = std::chrono::high_resolution_clock::now();
}

//	returns true if the timer is running
bool Stopwatch::isStarted() const
{
    return (m_start != std::chrono::high_resolution_clock::time_point::min());
}

//  stop the timer
void Stopwatch::stop()
{
    m_start = std::chrono::high_resolution_clock::time_point::min();
}

//	get the number of milliseconds since the timer was started
tick_t Stopwatch::getMs() {
    if (isStarted()) {
        std::chrono::duration<tick_t, std::milli> diff = std::chrono::high_resolution_clock::now() - m_start;
        return diff.count();
    }
    return 0;
}

int64_t Stopwatch::getTimestamp() {
    auto time_point = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::duration duration = time_point.time_since_epoch();

    return duration.count();
}

void Stopwatch::sleep(unsigned ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

} // end of namespace aif
