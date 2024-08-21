/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_DESCRIPTOR_H
#define AIF_DESCRIPTOR_H

#include <aif/base/Types.h>
#include <aif/facade/EdgeAIVision.h>
#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif
#include <rapidjson/document.h>

namespace aif {

class Descriptor
{
public:
    Descriptor();
    virtual ~Descriptor();

    virtual void addResponseName(
            const std::string& responseName,
            const std::string& id = "none");
    virtual void addReturnCode(int returnCode);
    std::string toStr();

    bool hasMember(const std::string& member) const;

    void initExtraOutput(const ExtraOutput& extraOutput);
    const ExtraOutput& getExtraOutput() const;

    bool addExtraOutput(ExtraOutputType type, void* buffer, size_t size);

protected:
    rapidjson::Document m_root;
    ExtraOutput m_extraOutput;

};

} // end of namespace aif

#endif // AIF_DESCRIPTOR_H
