/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PERFORMANCE_REPORTER_H
#define AIF_PERFORMANCE_REPORTER_H

#ifndef EDGEAI_VISION_REPORT_PATH
#define EDGEAI_VISION_REPORT_PATH "/tmp"
#endif

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
        FILE_WRITE_SETTING = 0X04, // not used as a report type
        REPORT_FILE = REPORT | FILE_WRITE_SETTING,
        DEFAULT_REPORT_TYPE = CONSOLE,
    };

    enum RecordType {
        CREATE_DETECTOR = 0,
        PREPROCESS,
        PROCESS,
        POSTPROCESS,
        NODEPROCESS,
        NUM_RECORD_TYPE,
    };

    enum TargetType {
        DETECTOR = 0,
        PIPENODE = 1,
    };

    enum {
        MAX_RECORD_SIZE = 101,
    };
};

class PerformanceRecorder
{
public:
    PerformanceRecorder(const std::string& name, const std::string& param,
                        Performance::TargetType type = Performance::TargetType::DETECTOR);
    virtual ~PerformanceRecorder();

    const std::string recordTypeToStr(Performance::RecordType type) noexcept;

    size_t getNum(Performance::RecordType type) const;
    void start(Performance::RecordType type);
    void stop(Performance::RecordType type);

    void printFirstInference(Performance::TargetType type);
    void printAverageInference(Performance::TargetType type);
    void printAverageInferenceExceptFirst(Performance::TargetType type);
    void printRawData(Performance::TargetType type);
    void printAll(bool showRawData, bool simpleReport = false);

private:
    std::string m_name;
    std::string m_param;
    Performance::TargetType m_targetType;
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
    void enableFileWriting();
    void disableFileWriting();
    void showSimpleReport();
    void showReport(bool showRawData = true);

private:
    PerformanceReporter() : m_reportType(Performance::DEFAULT_REPORT_TYPE) {}
    PerformanceReporter(const PerformanceReporter& other) : m_reportType(other.getReportType()) {}
    ~PerformanceReporter() {}

private:
    int m_reportType;
    std::string m_reportFileName = "edgeai-perf-report";
    std::unordered_map<std::string, std::shared_ptr<PerformanceRecorder>> m_recorders;

};

} // end of namespace aif

#endif // AIF_PERFORMANCE_REPORTER_H
