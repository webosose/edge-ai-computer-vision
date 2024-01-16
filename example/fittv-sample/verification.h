/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __VERIFICATION_H__
#define __VERIFICATION_H__

#include <aif/pipe/Pipe.h>

void verify_Dataset(aif::Pipe& pipe, const std::string& inputPath, bool saveToFiles);

#endif //__VERIFICATION_H__
