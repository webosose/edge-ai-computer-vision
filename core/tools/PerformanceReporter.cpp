/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/tools/PerformanceReporter.h>
#include <aif/log/Logger.h>
#include <aif/base/Types.h>
#include <numeric>
#include <iostream>
namespace aif {

PerformanceRecorder::PerformanceRecorder(
        const std::string& name, const std::string& param)
: m_name(name)
, m_param(param)
, m_stopwatch(Performance::NUM_RECORD_TYPE)
, m_time(Performance::NUM_RECORD_TYPE)
{
}

PerformanceRecorder::~PerformanceRecorder()
{
}

const std::string PerformanceRecorder::recordTypeToStr(Performance::RecordType type) noexcept
{
    switch (type) {
        case Performance::RecordType::CREATE_DETECTOR: return "CREATE_DETECTOR : ";
        case Performance::RecordType::PREPROCESS:      return "PREPROCESS      : ";
        case Performance::RecordType::PROCESS:         return "PROCESS         : ";
        case Performance::RecordType::POSTPROCESS:     return "POSTPROCESS     : ";
    }
    return "";
}

size_t PerformanceRecorder::getNum(Performance::RecordType type) const
{
    size_t utype = INT_TO_ULONG(type);
    if (utype < m_time.size()) {
        return m_time[utype].size();
    }
    return 0;
}

void PerformanceRecorder::start(Performance::RecordType type)
{
    if (PerformanceReporter::get().getReportType() == Performance::NONE) return;

    size_t utype = INT_TO_ULONG(type);
    if (utype < m_stopwatch.size()) {
        m_stopwatch[utype].start();
    }
}

void PerformanceRecorder::stop(Performance::RecordType type)
{

    size_t utype = INT_TO_ULONG(type);
    if (utype < m_stopwatch.size() && utype < m_time.size()) {
        int reportType = PerformanceReporter::get().getReportType();
        if (reportType == Performance::NONE) return;

        Stopwatch::tick_t tick = m_stopwatch[utype].getMs();
        if (reportType & Performance::CONSOLE) {
            Logd(recordTypeToStr(static_cast<Performance::RecordType>(type)), tick, "ms");
        }
        if ((reportType & Performance::REPORT) &&
                m_time[utype].size() < Performance::MAX_RECORD_SIZE) {
            m_time[utype].push_back(tick);
        }

        m_stopwatch[utype].stop();
    }
}

void PerformanceRecorder::printFirstInference()
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) return;

    Logd("[First inference]");
    for (size_t type = Performance::CREATE_DETECTOR; type < m_time.size(); type++) {
        if (m_time[type].size() == 0) {
            Logd(recordTypeToStr(static_cast<Performance::RecordType>(type)), "None");
        } else {
            Logd(recordTypeToStr(static_cast<Performance::RecordType>(type)), m_time[type][0], " ms");
        }
    }
    Logd("--------------------------------------------");
}

void PerformanceRecorder::printAverageInference()
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) return;

    Logd("[Average inference]");
    for (size_t type = Performance::PREPROCESS; type < m_time.size(); type++) {
        if (m_time[type].size() == 0) {
            Logd(recordTypeToStr(static_cast<Performance::RecordType>(type)), "None");
        } else {
            Stopwatch::tick_t sum =
                std::accumulate(std::begin(m_time[type]), std::end(m_time[type]), 0.0f);
            Logd(recordTypeToStr(static_cast<Performance::RecordType>(type)),
                 "count: ", m_time[type].size(),
                 ", avg: ", sum / m_time[type].size(), " ms");
        }
    }
    Logd("--------------------------------------------");
}

void PerformanceRecorder::printAverageInferenceExceptFirst()
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) return;

    Logd("[Average inference except first try]");
    for (size_t type = Performance::PREPROCESS; type < m_time.size(); type++) {
        if (m_time[type].size() <= 1) {
            Logd(recordTypeToStr(static_cast<Performance::RecordType>(type)), "None");
        } else {
            Stopwatch::tick_t sum =
                std::accumulate(std::begin(m_time[type]) + 1, std::end(m_time[type]), 0.0f);
            Logd(recordTypeToStr(static_cast<Performance::RecordType>(type)),
                    "count: ", m_time[type].size() - 1,
                    ", avg: ", sum / (m_time[type].size() - 1), " ms");
        }
    }
    Logd("--------------------------------------------");
}

void PerformanceRecorder::printAll(bool showRawData)
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) {
        Logd("Can not print report. Please change the ReportType to REPORT.");
        return;
    }
    Logd("Detector Report ============================");
    Logd("[Model] ", m_name);
    Logd("[Param] ", m_param);
    Logd("--------------------------------------------");
    printFirstInference();
    printAverageInference();
    printAverageInferenceExceptFirst();
    if (showRawData) {
        printRawData();
    }
}

void PerformanceRecorder::printRawData()
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) return;

    Logd("[Raw data]");
    for (size_t type = Performance::CREATE_DETECTOR; type < m_time.size(); type++) {
        std::cout << recordTypeToStr(static_cast<Performance::RecordType>(type));
        for (const auto& data : m_time[type]) {
            std::cout << data << ", ";
        }
        std::cout << std::endl;
    }
}

/////////////////////////////////////////////////////////////////////////////////
// PerformanceReporter Impl
void PerformanceReporter::addRecorder(
        const std::string& name, const std::shared_ptr<PerformanceRecorder>& recorder)
{
    m_recorders[name] = recorder;
}

void PerformanceReporter::removeRecorder(const std::string& name)
{
    if (m_recorders.find(name) != m_recorders.end()) {
        m_recorders.erase(name);
    }
}

Performance::ReportType PerformanceReporter::strToReportType(const std::string& type) noexcept
{
    if (type == "CONSOLE") {
        return Performance::CONSOLE;
    } else if (type == "REPORT") {

        return Performance::REPORT;
    }
    return Performance::NONE;
}

void PerformanceReporter::clear()
{
    m_reportType = Performance::NONE;
    m_recorders.clear();
}

void PerformanceReporter::showReport(bool showRawData)
{
    Logd("######### Performance Report ###############");
    for (auto& recorder : m_recorders) {
        recorder.second->printAll(showRawData);
    }
    Logd("############################################");
}

}