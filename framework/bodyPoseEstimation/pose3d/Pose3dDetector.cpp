#include <aif/bodyPoseEstimation/pose3d/Pose3dDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

Pose3dDetector::Pose3dDetector(const std::string& modelPath)
    : Detector(modelPath)
    , mMaxInputs(0)
    , mBatchSize(0)
    , mNumElems(0)
    , mNumJointsIn(0)
    , mNumJointsOut{0, 0}
    , mIsSecondDetect(false)
    , mScaleIn(0.0f)
    , mZeropointIn(0)
    , mScaleOut{0.0f, 0.0f}
    , mZeropointOut{0, 0}
    , mResultForFirstBatch{nullptr, nullptr}
{
}

Pose3dDetector::~Pose3dDetector()
{
    for (int i = 0; i < RESULT_3D_IDX_MAX; i++) {
        if (mResultForFirstBatch[i])
            ::free(mResultForFirstBatch[i]);
        }
}

Joint2D Pose3dDetector::normalizeJoints(const float x, const float y, const int width, const int height )
{
    Joint2D p;
    p.x = ( ( x / width ) * 2.0f ) - 1.0f;
    p.y = ( ( y / width ) * 2.0f ) -
                        ( static_cast<float>( height ) / static_cast<float>( width ) );
    return p;
}

t_aif_status Pose3dDetector::detect(const cv::Mat &img, std::shared_ptr<Descriptor> &descriptor)
{
    try {
        mIsSecondDetect = false;
        for (int i = 0; i < mBatchSize; i++) {
            Stopwatch sw;
            // prepare input tensors
            sw.start();
            if (fillInputTensor(img) != kAifOk) {
                throw std::runtime_error("fill input tensor failed!!");
            }
            TRACE("fillInputTensor(): ", sw.getMs(), "ms");
            sw.stop();

            // start to inference
            sw.start();
            TfLiteStatus res = m_interpreter->Invoke();
            if (res != kTfLiteOk) {
                throw std::runtime_error("tflite interpreter invoke failed!!");
            }
            TRACE("m_interpreter->Invoke(): ", sw.getMs(), "ms");
            sw.stop();

            // start to postProcess
            sw.start();
            if (postProcessing(img, descriptor) == kAifError) {
                throw std::runtime_error("postProcessing failed!!");
            }
            TRACE("postProcessing(): ", sw.getMs(), "ms");
            sw.stop();
            mIsSecondDetect = (mIsSecondDetect) ? false : true;
        }
        return kAifOk;
    } catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }
}

std::shared_ptr<DetectorParam> Pose3dDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<Pose3dParam>();
    return param;
}

void Pose3dDetector::setModelInfo(TfLiteTensor* inputTensor)
{
    m_modelInfo.batchSize = -1;  //inputTensor->dims->data[0];
    m_modelInfo.height = inputTensor->dims->data[0]; // mNumElems
    m_modelInfo.width = inputTensor->dims->data[1];  // mNumJointsIn
    m_modelInfo.channels = inputTensor->dims->data[2];

    mNumElems = m_modelInfo.height; // 27
    mNumJointsIn = m_modelInfo.width; // 41

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height(mNumElems):", m_modelInfo.height);
    TRACE("width(mNumJointsIn): ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);
}

t_aif_status Pose3dDetector::fillInputTensor(const cv::Mat& joints_mat)/* override*/
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

        if ( (joints_mat.rows != mNumJointsIn) || (joints_mat.cols != 2) ) /* 41 x 2 */
        {
            throw std::runtime_error("joints_mat input size is wrong!");
        }

        if (!mIsSecondDetect) {    // first batch
            Joints2D curr_joint_array;

            if ( param->preprocessingType == Pose3dParam::PreprocessingType::HOMOGENEOUS_COORDINATES )
            {
                cv::Mat undistort_result;
                cv::undistortPoints( joints_mat, undistort_result, mCameraMatrix, mDistCoeff ); /* remove distortion and change to CCS */
                for ( int j = 0; j < undistort_result.rows; j++ )
                {
                    Joint2D p;
                    p.x = undistort_result.at<double>( j, 0 );
                    p.y = undistort_result.at<double>( j, 1 );
                    curr_joint_array.push_back( p );
                }
            }
            else
            {
                for ( int j = 0; j < joints_mat.rows; j++ )
                {
                    float x = joints_mat.at<double>( j, 0 );
                    float y = joints_mat.at<double>( j, 1 );
                    Joint2D p = normalizeJoints( x, y, param->orgImgWidth, param->orgImgHeight );
                    curr_joint_array.push_back( p );
                }
            }

            mJoints_Q.push_back( curr_joint_array );
            if ( mJoints_Q.size() > mNumElems )
            {
                mJoints_Q.pop_front();
            }

            fillJoints(m_interpreter->typed_input_tensor<uint8_t>(0));
        } else if (param->flipPoses) {    // second batch for flip image
            fillFlippedJoints(m_interpreter->typed_input_tensor<uint8_t>(0));
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

t_aif_status Pose3dDetector::preProcessing()
{
    try {
        initializeParam();

        const std::vector<int> &inputs = m_interpreter->inputs();
        TfLiteTensor *input = m_interpreter->tensor(inputs[0]);
        getInputTensorInfo(input);

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

t_aif_status Pose3dDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    try {
        const std::vector<int> &outputs = m_interpreter->outputs();
        if (outputs.size() < 1 || outputs.size() > 2) throw std::runtime_error("output size should be 1 or 2!");

        std::vector<int> outputIdxs;
        for (int i = 0; i < outputs.size(); i++) {
            TfLiteTensor *output = m_interpreter->tensor(outputs[i]);
            int outputIdx = getOutputTensorInfo(output);
            outputIdxs.push_back(outputIdx);

            if (!mIsSecondDetect) {
                postProcess_forFirstBatch(outputIdx, output);
            } else {
                postProcess_forSecondBatch(outputIdx, output);
            }
        }

        if (mIsSecondDetect) {
            std::shared_ptr<Pose3dDescriptor> pose3dDescriptor = std::dynamic_pointer_cast<Pose3dDescriptor>(descriptor);
            if (pose3dDescriptor == nullptr) {
                throw std::runtime_error("failed to convert descriptor to Pose3dDescriptor");
            }

            for (int i = 0; i < outputIdxs.size(); i++) {
                if (outputIdxs[i] == RESULT_3D_JOINT) pose3dDescriptor->addJoints3D(mResults[RESULT_3D_JOINT]);
                else pose3dDescriptor->addTraj3D(mResults[RESULT_3D_TRAJ][0]);
            }
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

void Pose3dDetector::initializeParam()
{
    std::shared_ptr<Pose3dParam> param = std::dynamic_pointer_cast<Pose3dParam>(m_param);
    if (param == nullptr) {
        Loge(__func__, "failed to convert DetectorParam to Pose3dParam");
        return;
    }


    mMaxInputs = 1;
    mBatchSize = 1;

    if ( param->flipPoses == true )
    {
        mBatchSize *= 2;
    }

    if ( param->preprocessingType == Pose3dParam::PreprocessingType::HOMOGENEOUS_COORDINATES )
    {
        getCameraIntrinsics();
    }
}

void Pose3dDetector::getCameraIntrinsics()
{
    std::shared_ptr<Pose3dParam> param = std::dynamic_pointer_cast<Pose3dParam>(m_param);
    if (param == nullptr) {
        Loge(__func__, "failed to convert DetectorParam to Pose3dParam");
        return;
    }

    auto focal_length = param->focalLength;
    auto center = param->center;
    auto radialDistortion = param->radialDistortion;
    auto tanDistortion = param->tanDistortion;

    if ( param->hasIntrinsics == false)
    {
        focal_length.fill( 1000.0f );
        center[0] = param->orgImgWidth / 2.0f;
        center[1] = param->orgImgHeight / 2.0f;
        radialDistortion.fill( 0.0f );
        tanDistortion.fill( 0.0f );
    }

    mCameraMatrix = cv::Mat::zeros( cv::Size( 3, 3 ), CV_32FC1 );
    mCameraMatrix.at<float>( 0, 0 ) = focal_length[0];
    mCameraMatrix.at<float>( 1, 1 ) = focal_length[1];
    mCameraMatrix.at<float>( 0, 2 ) = center[0];
    mCameraMatrix.at<float>( 1, 2 ) = center[1];
    mCameraMatrix.at<float>( 2, 2 ) = 1;

    mDistCoeff = cv::Mat::zeros( cv::Size( 1, 5 ), CV_32FC1 );
    mDistCoeff.at<float>( 0, 0 ) = radialDistortion[0];
    mDistCoeff.at<float>( 0, 1 ) = radialDistortion[1];
    mDistCoeff.at<float>( 0, 2 ) = tanDistortion[0];
    mDistCoeff.at<float>( 0, 3 ) = tanDistortion[1];
    mDistCoeff.at<float>( 0, 4 ) = radialDistortion[2];

}

void Pose3dDetector::getInputTensorInfo(TfLiteTensor *input)
{
    if (input == nullptr || input->dims == nullptr) {
        throw std::runtime_error("input / input->dims is nullptr");
    }

    if (input->bytes != 27 * 41 * 2) {
        throw std::runtime_error("input bytes is wrong!!");
    }

    TfLiteAffineQuantization* q_params = reinterpret_cast<TfLiteAffineQuantization*>(input->quantization.params);
    if (!q_params) {
        throw std::runtime_error("input tensor doesn't have q_params...");
    }

    if (q_params->scale->size != 1) {
        throw std::runtime_error("input tensor should not per-axis quant...");
    }

    mScaleIn = q_params->scale->data[0];
    mZeropointIn = q_params->zero_point->data[0];

    TRACE( __func__, " mScaleIn: " , mScaleIn , " mZeropointIn: " , mZeropointIn);
}

int Pose3dDetector::getOutputTensorInfo(TfLiteTensor *output)
{
    if (output == nullptr || output->dims == nullptr) {
        throw std::runtime_error("output / output->dims is nullptr");
    }

    int idx = 0;
    if (output->bytes == 1 * 41 * 3 ) {
        idx = RESULT_3D_JOINT;
    } else if (output->bytes == 1 * 1 * 3 ) {
        idx = RESULT_3D_TRAJ;
    } else {
        throw std::runtime_error("output bytes is wrong!!");
    }

    TfLiteAffineQuantization* q_params = reinterpret_cast<TfLiteAffineQuantization*>(output->quantization.params);
    if (!q_params) {
        throw std::runtime_error("output tensor doesn't have q_params...");
    }

    if (q_params->scale->size != 1) {
        throw std::runtime_error("output tensor should not per-axis quant...");
    }

    mScaleOut[idx] = q_params->scale->data[0];
    mZeropointOut[idx] = q_params->zero_point->data[0];

    TRACE( __func__, " mScaleOut[" , idx , "]: " , mScaleOut[idx] , " mZeropointOut[" , idx , "]: " , mZeropointOut);
    return idx;
}

void Pose3dDetector::fillJoints(uint8_t* inputTensorBuff )
{
    TRACE(__func__);

    auto QUANT = [this](float data) {
        return static_cast<uint8_t>(std::max(std::min((( data / mScaleIn ) + mZeropointIn), 255.f), 0.0f));
    };

    // fill the oldest joints to the first line of buffer
    auto it = mJoints_Q.cbegin();
    auto prev_line = inputTensorBuff;
    int idx = 0;
    for ( auto& j2d : *it )  // x, y
    {
        // q = deq / scale + zero_point
        inputTensorBuff[2*idx] = QUANT(j2d.x);
        inputTensorBuff[2*idx + 1] = QUANT(j2d.y);
        idx++;
    }
    inputTensorBuff += mNumJointsIn * sizeof(uint8_t) * 2;
    it++;

    // copy to empty lines + 1
    const auto numEmptySlots = static_cast<int>( mNumElems ) - static_cast<int>( mJoints_Q.size() );
    const auto inByte = mNumJointsIn * sizeof(uint8_t) * 2; // 41 * 2 bytes? = 84 bytes
    for ( auto i = 0; i < numEmptySlots; i++ )
    {
        std::memcpy(inputTensorBuff, prev_line , inByte ); // copy prev_line's values to empty slots...
        prev_line = inputTensorBuff;
        inputTensorBuff += inByte;
    }

    // fill rest
    while ( it != mJoints_Q.cend() )
    {
        idx = 0;
        for ( auto& j2d : *it )
        {
            // q = deq / scale + zero_point TODO: ???
            inputTensorBuff[2*idx] = QUANT(j2d.x);
            inputTensorBuff[2*idx + 1] = QUANT(j2d.y);
            idx++;
        }
        inputTensorBuff += mNumJointsIn * sizeof(uint8_t) * 2;
        it++;
    }
}

void Pose3dDetector::fillFlippedJoints(uint8_t* inputTensorBuff)
{
    TRACE(__func__);

    std::shared_ptr<Pose3dParam> param = std::dynamic_pointer_cast<Pose3dParam>(m_param);
    if (param == nullptr) {
        Loge(__func__, "failed to convert DetectorParam to Pose3dParam");
        return;
    }

    auto QUANT = [this](float data) {
        return static_cast<uint8_t>(std::max(std::min((( data / mScaleIn ) + mZeropointIn), 255.f), 0.0f));
    };


    // fill the oldest joints to the first line of buffer
    auto it = mJoints_Q.cbegin();
    auto prev_line = inputTensorBuff;
    int idx = 0;
    for ( auto& j2d : *it )
    {
        // q = deq / scale + zero_point TODO: ???
        auto flipped_idx = param->flipPoseMap[idx];
        inputTensorBuff[2*flipped_idx] = QUANT((j2d.x) * -1.f);
        inputTensorBuff[2*flipped_idx + 1] = QUANT(j2d.y);
        idx++;
    }
    inputTensorBuff += mNumJointsIn * sizeof(uint8_t) * 2;
    it++;

    // copy to empty lines + 1
    const auto numEmptySlots = static_cast<int>( mNumElems ) - static_cast<int>( mJoints_Q.size() );
    const auto inByte = mNumJointsIn * sizeof(uint8_t) * 2; // 41 * 2 Bytes
    for ( auto i = 0; i < numEmptySlots; i++ )
    {
        std::memcpy(inputTensorBuff, prev_line , inByte ); // copy prev_line's values to empty slots...
        prev_line = inputTensorBuff;
        inputTensorBuff += inByte;
    }

    // fill rest
    while ( it != mJoints_Q.cend() )
    {
        idx = 0;
        for ( auto& j2d : *it )
        {
            auto flipped_idx = param->flipPoseMap[idx];
            inputTensorBuff[2*flipped_idx] = QUANT((j2d.x) * -1.f);
            inputTensorBuff[2*flipped_idx + 1] = QUANT(j2d.y);
            idx++;
        }
        inputTensorBuff += mNumJointsIn * sizeof(uint8_t) * 2;
        it++;
    }
}

void Pose3dDetector::postProcess_forFirstBatch(int outputIdx, TfLiteTensor* output)
{
    // first batch, just only save the value
    // alloc for saving it
    TRACE(__func__, "outputIdx: " , outputIdx, " alloc & memcpy with ", output->bytes, " bytes");
    if (outputIdx < 0 || outputIdx >= RESULT_3D_IDX_MAX) {
        Loge(__func__, "outputInx is out of range: ", outputIdx);
        return;
    }

    if (!mResultForFirstBatch[outputIdx]) {
        mResultForFirstBatch[outputIdx] = reinterpret_cast<uint8_t*>(std::malloc(output->bytes));
    }
    std::memcpy(mResultForFirstBatch[outputIdx], output->data.uint8, output->bytes);
}

void Pose3dDetector::postProcess_forSecondBatch(int outputIdx, TfLiteTensor* output)
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
    mNumJointsOut[outputIdx] = output->dims->data[1]; // 41 or 1
    averageWithFilippedResult(outputIdx, mResultForFirstBatch[outputIdx], output->data.uint8,
                              1 /*mNumInputs*/, mNumJointsOut[outputIdx]);
}

void Pose3dDetector::averageWithFilippedResult(int idx, uint8_t* buff, uint8_t* flipped, int numInputs, int numJoints)
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


    auto DEQ = [this, idx](uint8_t data) {
        return (mScaleOut[idx] * (data - mZeropointOut[idx]));
    };

    mResults[idx].clear();
    Joint3D result;
    for ( auto j = 0; j < numJoints; ++j) // 41, 1
    {
        auto f_idx = param->flipPoseMap[j];
        result.x = ( DEQ(buff[3*j]) + ( DEQ(flipped[3*f_idx]) * -1.f ) ) / 2;
        result.y = ( DEQ(buff[3*j+1]) + DEQ(flipped[3*f_idx+1]) ) / 2;
        result.z = ( DEQ(buff[3*j+2]) + DEQ(flipped[3*f_idx+2]) ) / 2;

        mResults[idx].push_back(result);
    }
}

} // end of namespace aif