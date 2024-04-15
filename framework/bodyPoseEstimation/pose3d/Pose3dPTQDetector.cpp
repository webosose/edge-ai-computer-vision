/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose3d/Pose3dPTQDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

template <typename T>
Pose3dPTQDetector<T>::Pose3dPTQDetector(const std::string& modelPath)
    : mResultForFirstBatch{nullptr, nullptr}
    , Pose3dDetector(modelPath)
{
}

template <typename T>
Pose3dPTQDetector<T>::~Pose3dPTQDetector()
{
    for (int i = 0; i < RESULT_3D_IDX_MAX; i++) {
        if (mResultForFirstBatch[i])
            ::free(mResultForFirstBatch[i]);
    }
}

template <typename T>
void Pose3dPTQDetector<T>::setModelInOutInfo(const std::vector<int> &t_inputs,
                                          const std::vector<int> &t_outputs)
{
    if (t_inputs.size() != 1) {
        Loge("input tensor size should be 1 ");
        return;
    }

    TfLiteTensor *inputTensor = m_interpreter->tensor(t_inputs[0]);
    if (inputTensor == nullptr || inputTensor->dims == nullptr) {
        Loge("tflite inputTensor invalid!!");
        return;
    }

    auto quant = getQuantizationTensorInfo(inputTensor);
    mScaleIn = quant.first;
    mZeropointIn = quant.second;

    TRACE("input  mScaleIn: " , mScaleIn , " mZeropointIn: " , mZeropointIn);

    m_modelInfo.inputSize = inputTensor->dims->size;
    m_modelInfo.batchSize = inputTensor->dims->data[0];
    m_modelInfo.height = inputTensor->dims->data[1]; // mNumElems
    m_modelInfo.width = inputTensor->dims->data[2];  // mNumJointsIn
    m_modelInfo.channels = inputTensor->dims->data[3];

    mNumElems = m_modelInfo.height; // 27
    mNumJointsIn = m_modelInfo.width; // 41

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height(mNumElems):", m_modelInfo.height);
    TRACE("width(mNumJointsIn): ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);


    if (t_outputs.size() != 1) {
        Loge("output tensor size should be 1!");
        return;
    }

    TfLiteTensor *outputTensor = m_interpreter->tensor(t_outputs[0]);

    int outputIdx;
    if (outputTensor->bytes == 1 * 1 * 41 * 3) {
        outputIdx = RESULT_3D_JOINT;
    } else if (outputTensor->bytes == 1 * 1 * 1 * 3) {
        outputIdx = RESULT_3D_TRAJ;
    } else {
        Loge("output tensor bytes is wrong!!");
        return;
    }

    auto quant_o = getQuantizationTensorInfo(outputTensor);
    mScaleOut[outputIdx] = quant_o.first;
    mZeropointOut[outputIdx] = quant_o.second;

    TRACE("output mScaleOut: " , mScaleOut[outputIdx] , " mZeropointOut: " , mZeropointOut[outputIdx]);
}

template <typename T>
t_aif_status Pose3dPTQDetector<T>::fillInputTensor(const cv::Mat& joints_mat)/* override*/
{
    try {
        std::shared_ptr<Pose3dParam> param = std::dynamic_pointer_cast<Pose3dParam>(m_param);
        if (param == nullptr) {
            Loge(__func__, "failed to convert DetectorParam to Pose3dParam");
            return kAifError;
        }


        if (m_interpreter == nullptr) {
            throw std::runtime_error("pose3d.tflite interpreter not initialized!!");
        }

        TRACE("Pose3D start with imageJoint[idx] ", joints_mat.at<double>(0,0));
        TRACE("Pose3D start with imageJoints[idx] ", joints_mat.at<double>(0,1));
        TRACE("Pose3D start with imageJoints[idx] ", joints_mat.at<double>(1,0));
        TRACE("Pose3D start with imageJoints[idx] ", joints_mat.at<double>(1,1));

        if ((joints_mat.rows != mNumJointsIn) || (joints_mat.cols != 2)) /* 41 x 2 */
        {
            throw std::runtime_error("joints_mat input size is wrong!");
        }

        if (!mIsSecondDetect) {    // first batch
            Joints2D curr_joint_array;

            if (param->preprocessingType == Pose3dParam::PreprocessingType::HOMOGENEOUS_COORDINATES)
            {
                cv::Mat undistort_result;
                cv::undistortPoints(joints_mat, undistort_result, mCameraMatrix, mDistCoeff); /* remove distortion and change to CCS */
                for (int j = 0; j < undistort_result.rows; j++)
                {
                    Joint2D p;
                    p.x = undistort_result.at<double>(j, 0);
                    p.y = undistort_result.at<double>(j, 1);
                    curr_joint_array.push_back(p);
                }
            }
            else
            {
                for (int j = 0; j < joints_mat.rows; j++)
                {
                    float x = joints_mat.at<double>(j, 0);
                    float y = joints_mat.at<double>(j, 1);
                    Joint2D p = normalizeJoints(x, y, param->orgImgWidth, param->orgImgHeight);
                    curr_joint_array.push_back(p);
                }
            }

            int NumUpdateInput = 1;
            if (param->alternativeInputType != Pose3dParam::AlternativeInputType::IGNORE)
            {
                NumUpdateInput = mNumSkippedInput + 1 > mNumElems ? mNumElems : mNumSkippedInput + 1;
            }

            // ignore, can not interpolate, replicate
            if ((NumUpdateInput == 1) || (mJoints_Q.size() == 0) || (param->alternativeInputType != Pose3dParam::AlternativeInputType::INTERPOLATE))
            {
                for (int j = 1; j <= NumUpdateInput; j++)
                {
                    mJoints_Q.push_back(curr_joint_array);
                }
            }
            else // interpolate
            {
                Joints2D last_joint_array = mJoints_Q.back();
                for (int j = 1; j <= NumUpdateInput; j++)
                {
                    mJoints_Q.push_back(interpolateJoints(last_joint_array, curr_joint_array, NumUpdateInput, j));
                }
            }

            while (mJoints_Q.size() > mNumElems)
            {
                mJoints_Q.pop_front();
            }

            fillJoints(m_interpreter->typed_input_tensor<T>(0));
        } else if (param->flipPoses) {    // second batch for flip image
            fillFlippedJoints(m_interpreter->typed_input_tensor<T>(0));
        } else {
            throw std::runtime_error("if mIsSecondDetect is true, flipPoses should be true");
        }

        Logv(__func__, "current mJoints_Q size is ", mJoints_Q.size());
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }

    return kAifOk;
}

template <typename T>
t_aif_status Pose3dPTQDetector<T>::preProcessing()
{
    try {
        initializeParam();

        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }

    return kAifOk;
}

template <typename T>
t_aif_status Pose3dPTQDetector<T>::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    try {
        const std::vector<int> &outputs = m_interpreter->outputs();
        if (outputs.size() != 1) throw std::runtime_error("output size should be 1");

        TfLiteTensor *output = m_interpreter->tensor(outputs[0]);
        if (output == nullptr || output->dims == nullptr) {
            throw std::runtime_error("output tensor is invalid!!");
        }

        int outputIdx;
        if (output->bytes == 1 * 1 * 41 * 3) { /* shape is different from V1, V2 models */
            outputIdx = RESULT_3D_JOINT;
        } else if (output->bytes == 1 * 1 * 1 * 3) {
            outputIdx = RESULT_3D_TRAJ;
        } else {
            throw std::runtime_error("output bytes is wrong!!");
        }

        if (!mIsSecondDetect) {
            postProcess_forFirstBatch(outputIdx, output);
        } else {
            postProcess_forSecondBatch(outputIdx, output);
        }

        if (mIsSecondDetect) {
            std::shared_ptr<Pose3dDescriptor> pose3dDescriptor = std::dynamic_pointer_cast<Pose3dDescriptor>(descriptor);
            if (pose3dDescriptor == nullptr) {
                throw std::runtime_error("failed to convert descriptor to Pose3dDescriptor");
            }

            if (outputIdx == RESULT_3D_JOINT) pose3dDescriptor->addJoints3D(mResults[RESULT_3D_JOINT]);
            else pose3dDescriptor->addTraj3D(mResults[RESULT_3D_TRAJ][0]);

        }

        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }

    return kAifOk;
}

template <typename T>
void Pose3dPTQDetector<T>::fillJoints(T* inputTensorBuff)
{
    TRACE(__func__);

    std::function<T(float)> QUANT;
    if (typeid(T) == typeid(int8_t)) {
        QUANT = [this](float data) {
            return static_cast<int8_t>(std::max(std::min(((data / mScaleIn) + mZeropointIn), 127.f), -128.f));
        };
    } else if (typeid(T) == typeid(uint8_t)) {
        QUANT = [this](float data) {
            return static_cast<uint8_t>(std::max(std::min(((data / mScaleIn) + mZeropointIn), 255.f), 0.f));
        };
    } else {
        throw std::runtime_error("T should be int8_t or uint8_t!!");
    }

    // fill the oldest joints to the first line of buffer
    auto it = mJoints_Q.cbegin();
    auto prev_line = inputTensorBuff;
    int idx = 0;
    for (auto& j2d : *it)  // x, y
    {
        // q = deq / scale + zero_point
        inputTensorBuff[2*idx] = QUANT(j2d.x);
        inputTensorBuff[2*idx + 1] = QUANT(j2d.y);
        idx++;
    }
    inputTensorBuff += mNumJointsIn * sizeof(T) * 2;
    it++;

    // copy to empty lines + 1
    const auto numEmptySlots = static_cast<int>(mNumElems) - static_cast<int>(mJoints_Q.size());
    const auto inByte = mNumJointsIn * sizeof(T) * 2; // 41 * 2 bytes? = 84 bytes
    for (auto i = 0; i < numEmptySlots; i++)
    {
        std::memcpy(inputTensorBuff, prev_line , inByte); // copy prev_line's values to empty slots...
        prev_line = inputTensorBuff;
        inputTensorBuff += inByte;
    }

    // fill rest
    while (it != mJoints_Q.cend())
    {
        idx = 0;
        for (auto& j2d : *it)
        {
            // q = deq / scale + zero_point TODO: ???
            inputTensorBuff[2*idx] = QUANT(j2d.x);
            inputTensorBuff[2*idx + 1] = QUANT(j2d.y);
            idx++;
        }
        inputTensorBuff += mNumJointsIn * sizeof(T) * 2;
        it++;
    }
}

template <typename T>
void Pose3dPTQDetector<T>::fillFlippedJoints(T* inputTensorBuff)
{
    TRACE(__func__);

    std::shared_ptr<Pose3dParam> param = std::dynamic_pointer_cast<Pose3dParam>(m_param);
    if (param == nullptr) {
        Loge(__func__, "failed to convert DetectorParam to Pose3dParam");
        return;
    }

    std::function <T(float)> QUANT;
    if (typeid(T) == typeid(int8_t)) {
        QUANT = [this](float data) {
            return static_cast<int8_t>(std::max(std::min(((data / mScaleIn) + mZeropointIn), 127.f), -128.f));
        };
    } else if (typeid(T) == typeid(uint8_t)) {
        QUANT = [this](float data) {
            return static_cast<uint8_t>(std::max(std::min(((data / mScaleIn) + mZeropointIn), 255.f), 0.f));
        };
    } else {
        throw std::runtime_error("T should be int8_t or uint8_t!!");
    }

    // fill the oldest joints to the first line of buffer
    auto it = mJoints_Q.cbegin();
    auto prev_line = inputTensorBuff;
    int idx = 0;
    for (auto& j2d : *it)
    {
        // q = deq / scale + zero_point TODO: ???
        auto flipped_idx = param->flipPoseMap[idx];
        inputTensorBuff[2*flipped_idx] = QUANT((j2d.x) * -1.f);
        inputTensorBuff[2*flipped_idx + 1] = QUANT(j2d.y);
        idx++;
    }
    inputTensorBuff += mNumJointsIn * sizeof(T) * 2;
    it++;

    // copy to empty lines + 1
    const auto numEmptySlots = static_cast<int>(mNumElems) - static_cast<int>(mJoints_Q.size());
    const auto inByte = mNumJointsIn * sizeof(T) * 2; // 41 * 2 Bytes
    for (auto i = 0; i < numEmptySlots; i++)
    {
        std::memcpy(inputTensorBuff, prev_line , inByte); // copy prev_line's values to empty slots...
        prev_line = inputTensorBuff;
        inputTensorBuff += inByte;
    }

    // fill rest
    while (it != mJoints_Q.cend())
    {
        idx = 0;
        for (auto& j2d : *it)
        {
            auto flipped_idx = param->flipPoseMap[idx];
            inputTensorBuff[2*flipped_idx] = QUANT((j2d.x) * -1.f);
            inputTensorBuff[2*flipped_idx + 1] = QUANT(j2d.y);
            idx++;
        }
        inputTensorBuff += mNumJointsIn * sizeof(T) * 2;
        it++;
    }
}

template <typename T>
void Pose3dPTQDetector<T>::postProcess_forFirstBatch(int outputIdx, TfLiteTensor* output)
{
    // first batch, just only save the value
    // alloc for saving it
    TRACE(__func__, "outputIdx: " , outputIdx, " alloc & memcpy with ", output->bytes, " bytes");
    if (outputIdx < 0 || outputIdx >= RESULT_3D_IDX_MAX) {
        Loge(__func__, "outputInx is out of range: ", outputIdx);
        return;
    }

    if (!mResultForFirstBatch[outputIdx]) {
        mResultForFirstBatch[outputIdx] = reinterpret_cast<T*>(std::malloc(output->bytes));
    }

    if (typeid(T) == typeid(int8_t)) {
        std::memcpy(mResultForFirstBatch[outputIdx], output->data.int8, output->bytes);
    } else if (typeid(T) == typeid(uint8_t)) {
        std::memcpy(mResultForFirstBatch[outputIdx], output->data.uint8, output->bytes);
    } else {
        throw std::runtime_error("T should be int8_t or uint8_t!!");
    }
}

template <typename T>
void Pose3dPTQDetector<T>::postProcess_forSecondBatch(int outputIdx, TfLiteTensor* output)
{
    TRACE(__func__, "outputIdx: " , outputIdx, " alloc & memcpy with ", output->bytes, " bytes");
    if (outputIdx < 0 || outputIdx >= RESULT_3D_IDX_MAX) {
        Loge(__func__, "outputInx is out of range: ", outputIdx);
        return;
    }


    std::shared_ptr<Pose3dParam> param = std::dynamic_pointer_cast<Pose3dParam>(m_param);
    if (param == nullptr) {
        Loge(__func__, "failed to convert DetectorParam to Pose3dParam");
        return;
    }


    if (!param->flipPoses) {
        throw std::runtime_error("Second Batch Detect can't be run without param->flipPoses!!");
    }

    // second batch, average 2 values
    mNumJointsOut[outputIdx] = output->dims->data[2]; // 41 or 1

    if (typeid(T) == typeid(int8_t)) {
        averageWithFilippedResult<int8_t>(outputIdx, mResultForFirstBatch[outputIdx], output->data.int8, m_modelInfo.batchSize, mNumJointsOut[outputIdx]);
    } else if (typeid(T) == typeid(uint8_t)) {
        averageWithFilippedResult<uint8_t>(outputIdx, mResultForFirstBatch[outputIdx],  output->data.uint8, m_modelInfo.batchSize, mNumJointsOut[outputIdx]);
    } else {
        throw std::runtime_error("T should be int8_t or uint8_t!!");
    }


}

template <typename T>
template <typename U>
void Pose3dPTQDetector<T>::averageWithFilippedResult(int idx, T* buff, U* flipped, int numInputs, int numJoints)
{
    if (idx < 0 || idx >= RESULT_3D_IDX_MAX) {
        Loge(__func__, "idx is out of range: ", idx);
        return;
    }

    std::shared_ptr<Pose3dParam> param = std::dynamic_pointer_cast<Pose3dParam>(m_param);
    if (param == nullptr) {
        Loge(__func__, "failed to convert DetectorParam to Pose3dParam");
        return;
    }


    auto DEQ = [this, idx](T data) {
        return (mScaleOut[idx] * (data - mZeropointOut[idx]));
    };

    mResults[idx].clear();
    Joint3D result;
    for (auto j = 0; j < numJoints; ++j) // 41, 1
    {
        auto f_idx = param->flipPoseMap[j];
        result.x = (DEQ(buff[3*j]) + (DEQ(flipped[3*f_idx]) * -1.f)) / 2;
        result.y = (DEQ(buff[3*j+1]) + DEQ(flipped[3*f_idx+1])) / 2;
        result.z = (DEQ(buff[3*j+2]) + DEQ(flipped[3*f_idx+2])) / 2;

        mResults[idx].push_back(result);
    }
}

} // end of namespace aif

template class aif::Pose3dPTQDetector<int8_t>;
template class aif::Pose3dPTQDetector<uint8_t>;