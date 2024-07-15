/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once
#include <aif/base/Types.h>

#include <initializer_list>

namespace aif
{

class ProcessRunner
{
public:
  ProcessRunner(std::string cmd, std::initializer_list<std::string> args) noexcept;
  ~ProcessRunner() = default;
  std::string getResult() const { return m_result; }
  int getExitCode() const { return m_exitCode; }

private:
  pid_t m_pid;
  std::string m_result;
  int m_exitCode;
};

} // end of namespace aif