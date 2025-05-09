/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */


#include <aif/mname/MNameDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

MNameDescriptor::MNameDescriptor()
    : Descriptor()
{
}

MNameDescriptor::~MNameDescriptor()
{
}

}
