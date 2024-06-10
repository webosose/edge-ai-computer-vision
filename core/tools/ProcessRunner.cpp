/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/tools/ProcessRunner.h>

#include <boost/process.hpp>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

namespace bp = boost::process;

namespace aif
{

ProcessRunner::ProcessRunner(std::string cmd, std::initializer_list<std::string> args)
    : m_pid(0), m_result("")
{
    std::string arg = "";
    for (const auto &a : args) {
        arg += a + " ";
    }
    Logi("ProcessRunner::ProcessRunner(", cmd, " ", arg, ")");
    try {
      bp::ipstream outStream;
      bp::child c(cmd + " " + arg, bp::std_out > outStream);

      std::string line;
      while (outStream && std::getline(outStream, line) && !line.empty()) {
          m_result += line + '\n';
      }
      Logi("ProcessRunner::ProcessRunner() result: ", m_result);

      c.wait();
    } catch(const std::exception &e) {
      Loge("Failed to execute command: ", cmd + " " + arg, " Error: ", e.what());
    }
}

} // end of namespace aif