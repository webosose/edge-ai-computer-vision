/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/tools/PerformanceReporter.h>
#include <aif/log/Logger.h>
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

int PerformanceRecorder::getNum(Performance::RecordType type) const
{
    return m_time[type].size();
}

void PerformanceRecorder::start(Performance::RecordType type)
{
    if (PerformanceReporter::get().getReportType() == Performance::NONE) return;

    m_stopwatch[type].start();
}

void PerformanceRecorder::stop(Performance::RecordType type)
{
    int reportType = PerformanceReporter::get().getReportType();
    if (reportType == Performance::NONE) return;

    Stopwatch::tick_t tick = m_stopwatch[type].getMs();
    if (reportType & Performance::CONSOLE) {
        Logi(recordTypeToStr(static_cast<Performance::RecordType>(type)), tick, "ms");
    }
    if ((reportType & Performance::REPORT) &&
            m_time[type].size() < Performance::MAX_RECORD_SIZE) {
       m_time[type].push_back(tick);
    }

    m_stopwatch[type].stop();
}

void PerformanceRecorder::printFirstInference()
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) return;

    Logi("[First inference]");
    for (int type = Performance::CREATE_DETECTOR; type < Performance::NUM_RECORD_TYPE; type++) {
        if (m_time[type].size() == 0) {
            Logi(recordTypeToStr(static_cast<Performance::RecordType>(type)), "None");
        } else {
            Logi(recordTypeToStr(static_cast<Performance::RecordType>(type)), m_time[type][0], " ms");
        }
    }
    Logi("--------------------------------------------");
}

void PerformanceRecorder::printAverageInference()
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) return;

    Logi("[Average inference]");
    for (int type = Performance::PREPROCESS; type < Performance::NUM_RECORD_TYPE; type++) {
        if (m_time[type].size() == 0) {
            Logi(recordTypeToStr(static_cast<Performance::RecordType>(type)), "None");
        } else {
            Stopwatch::tick_t sum =
                std::accumulate(std::begin(m_time[type]), std::end(m_time[type]), 0.0f);
            Logi(recordTypeToStr(static_cast<Performance::RecordType>(type)),
                 "count: ", m_time[type].size(),
                 ", avg: ", sum / m_time[type].size(), " ms");
        }
    }
    Logi("--------------------------------------------");
}

void PerformanceRecorder::printAverageInferenceExceptFirst()
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) return;

    Logi("[Average inference except first try]");
    for (int type = Performance::PREPROCESS; type < Performance::NUM_RECORD_TYPE; type++) {
        if (m_time[type].size() <= 1) {
            Logi(recordTypeToStr(static_cast<Performance::RecordType>(type)), "None");
        } else {
            Stopwatch::tick_t sum =
                std::accumulate(std::begin(m_time[type]) + 1, std::end(m_time[type]), 0.0f);
            Logi(recordTypeToStr(static_cast<Performance::RecordType>(type)),
                    "count: ", m_time[type].size() - 1,
                    ", avg: ", sum / (m_time[type].size() - 1), " ms");
        }
    }
    Logi("--------------------------------------------");
}

void PerformanceRecorder::printAll(bool showRawData)
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) {
        Logi("Can not print report. Please change the ReportType to REPORT.");
        return;
    }
    Logi("Detector Report ============================");
    Logi("[Model] ", m_name);
    Logi("[Param] ", m_param);
    Logi("--------------------------------------------");
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

    Logi("[Raw data]");
    for (int type = Performance::CREATE_DETECTOR; type < Performance::NUM_RECORD_TYPE; type++) {
        std::cout << recordTypeToStr(static_cast<Performance::RecordType>(type));
        for (const auto& data : m_time[type]) {
            std::cout << data << ", ";
        }
        std::cout << std::endl;
    }
}

/////////////////////////////////////////////////////////////////////////////////
// PerformanceReporter Impl
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
    Logi("######### Performance Report ###############");
    for (const std::shared_ptr<PerformanceRecorder>& recorder : m_recorders) {
        recorder->printAll(showRawData);
    }
    Logi("############################################");
}

}