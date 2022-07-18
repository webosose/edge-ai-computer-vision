/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_MNAME_DESCRIPTOR_H
#define AIF_MNAME_DESCRIPTOR_H

#include <aif/base/Descriptor.h>
#include <aif/base/Types.h>

namespace aif {

class MNameDescriptor: public Descriptor
{
public:
    MNameDescriptor();
    virtual ~MNameDescriptor();

    // TODO: implment add result
};

} // end of namespace aif

#endif // AIF_MNAME_DESCRIPTOR_H
