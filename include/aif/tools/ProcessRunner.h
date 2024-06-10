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
  ProcessRunner(std::string cmd, std::initializer_list<std::string> args);
  ~ProcessRunner() = default;
  std::string getResult() const { return m_result; }

private:
  pid_t m_pid;
  std::string m_result;
};

} // end of namespace aif