/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __VERIFICATION_H__
#define __VERIFICATION_H__

#include <aif/pipe/Pipe.h>

void drawResults(const std::string& inputPath, const std::string & outputPath, const aif::Pipe& pipe, bool drawAll = true);
void verify_Dataset(aif::Pipe& pipe, const std::string& inputPath, const std::string& outputPath);

#endif //__VERIFICATION_H__
