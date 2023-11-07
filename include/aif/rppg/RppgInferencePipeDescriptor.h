/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_RPPG_INFERENCE_PIPE_DESCRIPTOR_H
#define AIF_RPPG_INFERENCE_PIPE_DESCRIPTOR_H

#include <aif/pipe/PipeDescriptor.h>
#include <aif/rppg/RppgDescriptor.h>
#include <aif/log/Logger.h>

namespace aif {

class RppgInferencePipeDescriptor : public PipeDescriptor
{
    private:
        RppgInferencePipeDescriptor();

    public:
        virtual ~RppgInferencePipeDescriptor();

        template<typename T>
            friend class PipeDescriptorFactoryRegistration;

        bool addBridgeOperationResult(
                const std::string& nodeId,
                const std::string& operationType,
                const std::string& result) override;

        bool addDetectorOperationResult(
                const std::string& nodeId,
                const std::string& model,
                const std::shared_ptr<Descriptor>& descriptor) override;

        void addBatchSize(const int& batchSize) { m_batchSize = batchSize; }
        void addChannelSize (const int& channelSize) { m_channelSize = channelSize; }
        void addBpm (const float& bpm) { m_bpm = bpm; }
        void addSignalCondition (const std::string& signalCondition) { m_signalCondition = signalCondition; }

        const std::vector<float>& getRppgOutputs() { return m_rppgOutputs; }
        const int getBatchSize() const { return m_batchSize; }
        const int getChannelSize() const { return m_channelSize; }
        const size_t getNumRppg() const { return m_rppgOutputs.size(); }
        bool addRppgFinalResult(float bpm, std::string signalCondition);

   private:
        bool appendRppg(
                const std::string& nodeId,
                const std::shared_ptr<RppgDescriptor> descriptor);

    private:
        std::vector<float> m_rppgOutputs;
        int m_batchSize;
        int m_channelSize;
        float m_bpm;
        std::string m_signalCondition;

};

} // end of namespace aif

#endif // AIF_RPPG_INFERENCE_PIPE_DESCRIPTOR_H
