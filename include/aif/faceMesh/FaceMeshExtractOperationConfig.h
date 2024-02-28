/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FACE_MESH_EXTRACT_OPERATION_CONFIG_H
#define AIF_FACE_MESH_EXTRACT_OPERATION_CONFIG_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <aif/pipe/PipeConfig.h>
#include <rapidjson/document.h>

namespace rj = rapidjson;
namespace aif {

class FaceMeshExtractOperationConfig : public BridgeOperationConfig {
    public:
        FaceMeshExtractOperationConfig();
        virtual ~FaceMeshExtractOperationConfig() {}

        bool parse(const rj::Value& value) override;
        const bool& getRGBextractOn() const { return m_rgbExtract; }
    private:
        bool m_rgbExtract;
};

} // end of namespace aif

#endif // AIF_FACE_MESH_EXTRACT_OPERATION_CONFIG_H
