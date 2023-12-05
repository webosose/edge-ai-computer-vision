/*
* Copyright (c) 2023 LG Electronics Inc.
* SPDX-License-Identifier: Apache-2.0
*/

#include <aif/rppg/RppgDetector.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>
#include <aif/rppg/CubicSpline.h>
#include <aif/rppg/RppgInferencePipeDescriptor.h>

#include <cmath>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <typeinfo>
#include <xtensor/xsort.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xbuilder.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xcomplex.hpp>


namespace aif {

RppgDetector::RppgDetector(const std::string& modelPath)
: Detector(modelPath)
{
}

RppgDetector::~RppgDetector()
{}

std::shared_ptr<DetectorParam> RppgDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<RppgParam>();
    return param;
}

void RppgDetector::setModelInfo(TfLiteTensor* inputTensor)
{
    if (inputTensor == nullptr) {
        Loge("inputTensor ptr is null");
        return;
    }
    m_modelInfo.batchSize = 1; //inputTensor->dims->data[0];
    m_modelInfo.height = inputTensor->dims->data[1]; // 400
    m_modelInfo.width = inputTensor->dims->data[2]; // 2

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
}

t_aif_status RppgDetector::preProcessing()
{
    return kAifOk;
}

} // end of namespace aif