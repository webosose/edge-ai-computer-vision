/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PERFORMANCE_REPORTER_H
#define AIF_PERFORMANCE_REPORTER_H

#include <aif/tools/Stopwatch.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace aif {

class Performance {
public:
    enum ReportType {
        NONE = 0x00,
        CONSOLE = 0x01,
        REPORT = 0x02,
        DEFAULT_REPORT_TYPE = CONSOLE,
    };

    enum RecordType {
        CREATE_DETECTOR = 0,
        PREPROCESS,
        PROCESS,
        POSTPROCESS,
        NUM_RECORD_TYPE,
    };

    enum {
        MAX_RECORD_SIZE = 101,
    };
};

class PerformanceRecorder
{
public:
    PerformanceRecorder(const std::string& name, const std::string& param);
    virtual ~PerformanceRecorder();

    const std::string recordTypeToStr(Performance::RecordType type) noexcept;

    int getNum(Performance::RecordType type) const;
    void start(Performance::RecordType type);
    void stop(Performance::RecordType type);

    void printFirstInference();
    void printAverageInference();
    void printAverageInferenceExceptFirst();
    void printRawData();
    void printAll(bool showRawData);

private:
    std::string m_name;
    std::string m_param;
    std::vector<Stopwatch> m_stopwatch;
    std::vector<std::vector<Stopwatch::tick_t>> m_time;

};

class PerformanceReporter
{
public:
    static PerformanceReporter& get() {
        static PerformanceReporter s_instance;
        return s_instance;
    }
    static Performance::ReportType strToReportType(const std::string& type) noexcept;

    void clear();
    void addReportType(int type) { m_reportType |= type; }
    int getReportType() const { return m_reportType; }
    void addRecorder(const std::string& name,
            const std::shared_ptr<PerformanceRecorder>& recorder);
    void removeRecorder(const std::string& name);
    void showReport(bool showRawData = true);

private:
    PerformanceReporter() : m_reportType(Performance::DEFAULT_REPORT_TYPE) {}
    PerformanceReporter(const PerformanceReporter& other) : m_reportType(other.getReportType()) {}
    ~PerformanceReporter() {}

private:
    int m_reportType;
    std::unordered_map<std::string, std::shared_ptr<PerformanceRecorder>> m_recorders;

};

} // end of namespace aif

#endif // AIF_PERFORMANCE_REPORTER_H
