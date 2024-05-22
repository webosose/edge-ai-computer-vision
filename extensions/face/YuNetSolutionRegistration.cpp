/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/face/YuNetSolutionRegistration.h>
#include <aif/tools/MakeConfig.h>
#include <aif/log/Logger.h>
#include <nlohmann/json.hpp>

using namespace nlohmann;


namespace aif {

std::string FaceSolution::makeSolutionConfig(SolutionConfig config)
{
    json param;

    param["commonParam"]["numMaxPerson"] = config.numMaxPerson;
    if (config.roi_region[0] > 0 || config.roi_region[1] > 0 ||
        config.roi_region[2] > 0 || config.roi_region[3] > 0)
    {
        param["modelParam"]["origImgRoiX"] = config.roi_region[0];
        param["modelParam"]["origImgRoiY"] = config.roi_region[1];
        param["modelParam"]["origImgRoiWidth"] = config.roi_region[2];
        param["modelParam"]["origImgRoiHeight"] = config.roi_region[3];
    }

#if defined(USE_NNAPI) /* K24 */ // TODO: naming again !!!!
    /* TODO: use autodelegate instead of nnapi delegate */
    param["delegates"] = json::array({{
        {"name", "nnapi_delegate"},
        {"option", {
            {"cache_dir", "/usr/share/aif/model_caches/"},
            {"model_token", "yunet_360_640_f32_model"}
        }}
    }});

#else /* default */
    param["autoDelegate"]["policy"] = "PYTORCH_MODEL_GPU";
    param["autoDelegate"]["cpu_fallback_percentage"] = 15;

#endif

    param["modelParam"]["scoreThreshold"] = 0.7;
    param["modelParam"]["nmsThreshold"] = 0.3;
    param["modelParam"]["topK"] = 5000;

    return makeDetectConfig("face_yunet_360_640", param);
}

} // end of namespace aif
