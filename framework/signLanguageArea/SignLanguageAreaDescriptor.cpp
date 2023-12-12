/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/signLanguageArea/SignLanguageAreaDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

SignLanguageAreaDescriptor::SignLanguageAreaDescriptor()
    : Descriptor()
{
}

SignLanguageAreaDescriptor::~SignLanguageAreaDescriptor()
{
}

bool SignLanguageAreaDescriptor::addBoxes(const std::vector<float> &outputs)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("signLanguageAreas")) {
        rj::Value signLanguageAreas(rj::kArrayType);
        m_root.AddMember("signLanguageAreas", signLanguageAreas, allocator);
    }
    rj::Value signLanguageAreas(rj::kArrayType);

    for (const auto &f : outputs)
        signLanguageAreas.PushBack(f, allocator);

    m_root["signLanguageAreas"] = std::move(signLanguageAreas);

    return true;
}

}
