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
        const std::string& name, const std::string& param, Performance::TargetType type)
: m_name(name)
, m_param(param)
, m_targetType(type)
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
        case Performance::RecordType::NODEPROCESS:     return "NODEPROCESS     : ";
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

void PerformanceRecorder::printFirstInference(Performance::TargetType target)
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) return;

    Logw("[First inference]");
    size_t st_pos, end_pos = 0;

    if (target == Performance::TargetType::DETECTOR) {
        st_pos = Performance::CREATE_DETECTOR;
        end_pos = Performance::POSTPROCESS;
    } else {
        st_pos = Performance::NODEPROCESS;
        end_pos = Performance::NODEPROCESS;
    }

    for (size_t type = st_pos; type < end_pos + 1; type++) {
        if (m_time[type].size() == 0) {
            Logw(recordTypeToStr(static_cast<Performance::RecordType>(type)), "None");
        } else {
            Logw(recordTypeToStr(static_cast<Performance::RecordType>(type)), m_time[type][0], " ms");
        }
    }
    Logw("--------------------------------------------");
}

void PerformanceRecorder::printAverageInference(Performance::TargetType target)
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) return;

    Logw("[Average inference]");
    size_t st_pos, end_pos = 0;

    if (target == Performance::TargetType::DETECTOR) {
        st_pos = Performance::PREPROCESS;
        end_pos = Performance::POSTPROCESS;
    } else {
        st_pos = Performance::NODEPROCESS;
        end_pos = Performance::NODEPROCESS;
    }

    for (size_t type = st_pos; type < end_pos + 1; type++) {
        if (m_time[type].size() == 0) {
            Logw(recordTypeToStr(static_cast<Performance::RecordType>(type)), "None");
        } else {
            Stopwatch::tick_t sum =
                std::accumulate(std::begin(m_time[type]), std::end(m_time[type]), 0.0f);
            Logw(recordTypeToStr(static_cast<Performance::RecordType>(type)),
                 "count: ", m_time[type].size(),
                 ", avg: ", sum / m_time[type].size(), " ms");
        }
    }
    Logw("--------------------------------------------");
}

void PerformanceRecorder::printAverageInferenceExceptFirst(Performance::TargetType target)
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) return;

    Logw("[Average inference except first try]");
    size_t st_pos, end_pos = 0;

    if (target == Performance::TargetType::DETECTOR) {
        st_pos = Performance::PREPROCESS;
        end_pos = Performance::POSTPROCESS;
    } else {
        st_pos = Performance::NODEPROCESS;
        end_pos = Performance::NODEPROCESS;
    }

    for (size_t type = st_pos; type < end_pos + 1; type++) {
        if (m_time[type].size() <= 1) {
            Logw(recordTypeToStr(static_cast<Performance::RecordType>(type)), "None");
        } else {
            Stopwatch::tick_t sum =
                std::accumulate(std::begin(m_time[type]) + 1, std::end(m_time[type]), 0.0f);
            Logw(recordTypeToStr(static_cast<Performance::RecordType>(type)),
                    "count: ", m_time[type].size() - 1,
                    ", avg: ", sum / (m_time[type].size() - 1), " ms");
        }
    }
    Logw("--------------------------------------------");
}

void PerformanceRecorder::printAll(bool showRawData, bool simpleReport)
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) {
        Loge("Can not print report. Please change the ReportType to REPORT.");
        return;
    }
    if (m_targetType == Performance::TargetType::DETECTOR) {
        Logw("============ Detector Report ===============");
        Logw("[Model] ", m_name);
        Logw("[Param] ", m_param);
    } else {
        Logw("============ PipeNode Report ===============");
        Logw("[PipeNode] (", m_name, ")");
    }
    Logw("--------------------------------------------");
    if (!simpleReport) {
        printFirstInference(m_targetType);
        printAverageInference(m_targetType);
    }
    printAverageInferenceExceptFirst(m_targetType);
    if (showRawData) {
        printRawData(m_targetType);
    }
    Logw("\n");
}

void PerformanceRecorder::printRawData(Performance::TargetType target)
{
    if (!(PerformanceReporter::get().getReportType() & Performance::REPORT)) return;

    Logd("[Raw data]");
    size_t st_pos, end_pos = 0;

    if (target == Performance::TargetType::DETECTOR) {
        st_pos = Performance::CREATE_DETECTOR;
        end_pos = Performance::POSTPROCESS;
    } else {
        st_pos = Performance::NODEPROCESS;
        end_pos = Performance::NODEPROCESS;
    }

    for (size_t type = st_pos; type < end_pos + 1; type++) {
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

void PerformanceReporter::showSimpleReport()
{
    if (!(m_reportType & Performance::REPORT)) return;

    Logw("######### Performance Simple Report ###############");
    for (auto& recorder : m_recorders) {
        recorder.second->printAll(false, true);
    }
    Logw("############################################");
}


void PerformanceReporter::showReport(bool showRawData)
{
    if (!(m_reportType & Performance::REPORT)) return;

    Logw("######### Performance Report ###############");
    for (auto& recorder : m_recorders) {
        recorder.second->printAll(showRawData);
    }
    Logw("############################################");
}

}
