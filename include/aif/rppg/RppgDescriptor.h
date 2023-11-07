/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_RPPG_DESCRIPTOR_H
#define AIF_RPPG_DESCRIPTOR_H

#include <aif/base/Descriptor.h>

#include <vector>

namespace aif {

class RppgDescriptor: public Descriptor
{
public:
    RppgDescriptor();
    virtual ~RppgDescriptor();

    void addRppgOutput(std::vector<float> outputs);
    void clear();

    void addRppg(std::vector<float>& rppgOutput) { m_rppgOut = std::move(rppgOutput); }
    void addBatchSize(int batchSize) { m_batchSize = batchSize; }
    void addChannelSize (int channelSize) { m_channelSize = channelSize; }

    const std::vector<float>& getRppg() const { return m_rppgOut; }
    size_t getRppgSize() const { return m_rppgOut.size(); }
    int getBatchSize() const { return m_batchSize; }
    int getChannelSize() const { return m_channelSize; }

protected:
    std::vector<float> m_rppgOut;
    int m_batchSize;
    int m_channelSize;
};

} // end of namespace aif

#endif // AIF_RPPG_DESCRIPTOR_H
