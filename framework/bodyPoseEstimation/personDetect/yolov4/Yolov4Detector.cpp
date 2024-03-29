#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

Yolov4Detector::Yolov4Detector(const std::string& modelPath)
    : PersonDetector(modelPath)
{
}

Yolov4Detector::~Yolov4Detector()
{
}

std::shared_ptr<DetectorParam> Yolov4Detector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<Yolov4Param>();
    return param;
}

void Yolov4Detector::setModelInfo(TfLiteTensor* inputTensor)
{
    m_modelInfo.batchSize = 1;
    m_modelInfo.height = inputTensor->dims->data[0];
    m_modelInfo.width = inputTensor->dims->data[1];
    m_modelInfo.channels = inputTensor->dims->data[2];

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);
}

t_aif_status Yolov4Detector::fillInputTensor(const cv::Mat& img)/* override*/
{
    if (img.rows == 0 || img.cols == 0) {
        Loge(__func__, " invalid opencv image!!");
        return kAifError;
    }

    int height = m_modelInfo.height;
    int width = m_modelInfo.width;
    int channels = m_modelInfo.channels;

    if (m_interpreter == nullptr) {
        Loge(__func__, " yolov4.tflite interpreter not initialized!!");
        return kAifError;
    }

    cv::Mat img_resized;
    if ( isRoiValid(img.cols, img.rows) ) {
        cv::Mat roi_img = img( cv::Rect(mOrigImgRoiX, mOrigImgRoiY, mOrigImgRoiWidth, mOrigImgRoiHeight) );
        img_resized = staticResize(roi_img, width, height);
    } else {
        img_resized = staticResize(img, width, height);
    }

    TRACE("resized size: ", img_resized.size());
    if (img_resized.rows != height || img_resized.cols != width) {
        Loge(__func__, " image resize failed!!");
        return kAifError;
    }

    img_resized.convertTo(img_resized, CV_8U);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            const auto &rgb = img_resized.at<cv::Vec3b>(i, j);
            m_interpreter->typed_input_tensor<uint8_t>(0)[i * width * channels + j * channels + 0] = rgb[2];
            m_interpreter->typed_input_tensor<uint8_t>(0)[i * width * channels + j * channels + 1] = rgb[1];
            m_interpreter->typed_input_tensor<uint8_t>(0)[i * width * channels + j * channels + 2] = rgb[0];
        }
    }
    return kAifOk;
}

t_aif_status Yolov4Detector::preProcessing()
{
    std::shared_ptr<Yolov4Param> param = std::dynamic_pointer_cast<Yolov4Param>(m_param);
    if (param == nullptr) {
        Loge(__func__, " failed to convert DetectorParam to Yolov4Param");
        return kAifError;
    }

    mOrigImgRoiX = param->origImgRoiX;
    mOrigImgRoiY = param->origImgRoiY;
    mOrigImgRoiWidth = param->origImgRoiWidth;
    mOrigImgRoiHeight = param->origImgRoiHeight;

    return kAifOk;
}

t_aif_status Yolov4Detector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    float *outConcatTensor_deq = nullptr; /* for dequantized output tensors */

    Stopwatch sw;
    sw.start();

    std::shared_ptr<Yolov4Param> param = std::dynamic_pointer_cast<Yolov4Param>(m_param);
    if (param == nullptr) {
        Loge(__func__, " failed to convert DetectorParam to Yolov4Param");
        return kAifError;
    }

    std::shared_ptr<Yolov4Descriptor> yolov4Descriptor = std::dynamic_pointer_cast<Yolov4Descriptor>(descriptor);
    if (yolov4Descriptor == nullptr) {
        Loge(__func__, " failed to convert Descriptor to Yolov4Descriptor");
        return kAifError;
    }


    uint8_t* outTensorDataArr[2];
    size_t outConcatDimSize = 0;

    t_aif_status res = getOutputTensorInfo(outTensorDataArr, outConcatDimSize);
    if (res != kAifOk) {
        return res;
    }

    size_t outConcatTotalSize = outConcatDimSize * sizeof(float);

    // alloc for new dequantized concated outTensor
    outConcatTensor_deq = reinterpret_cast<float*>(std::malloc(outConcatTotalSize));
    if (outConcatTensor_deq == nullptr) {
        Loge(__func__, " can't get new dequantized output memory !!");
        return kAifError;
    }

    TRACE(__func__, " outConcatTotalSize: ", outConcatTotalSize, " outConcatDimSize: " , outConcatDimSize, " outConcatTensor_deq: ", outConcatTensor_deq);
    res = transformToImageCoordinates(outTensorDataArr, outConcatTensor_deq, param);
    if (res != kAifOk) {
        ::free(outConcatTensor_deq);
        return kAifError;
    }

    std::vector<Object> proposals;
    generateYolov4Proposals( proposals , param, outConcatTensor_deq, outConcatDimSize);
    qsortDescentInplace( proposals );

    std::vector<unsigned int> picked;
    nmsSortedBboxes( param, proposals, picked );

    size_t count = picked.size();
    auto img_width = static_cast<float>(img.cols);
    auto img_height = static_cast<float>(img.rows);
    auto off_x = 0.0f;
    auto off_y = 0.0f;
    auto roi_img_width = img_width;
    auto roi_img_height = img_height;
    if (isRoiValid(img_width, img_height)) {
        off_x = static_cast<float>( mOrigImgRoiX );
        off_y = static_cast<float>( mOrigImgRoiY );
        roi_img_width = static_cast<float>( mOrigImgRoiWidth );
        roi_img_height = static_cast<float>( mOrigImgRoiHeight );
    }
    auto rx = static_cast<float>(m_modelInfo.width) / roi_img_width;
    auto ry = static_cast<float>(m_modelInfo.height) / roi_img_height;
    auto scale = std::min( rx, ry );

    TRACE(__func__, " count: ", count);
    std::vector<std::pair<BBox,float>> before_filtered;
    for (auto i = 0; i < count; i++ )
    {
        auto obj = proposals[picked[i]];

        float x0 = ( ( obj.rect.x ) / scale ) + off_x;
        float y0 = ( ( obj.rect.y ) / scale ) + off_y;
        float x1 = ( ( obj.rect.x + obj.rect.width ) / scale ) + off_x;
        float y1 = ( ( obj.rect.y + obj.rect.height ) / scale ) + off_y;
        x0 = std::max( std::min( x0, img_width - 1.0f ), 0.f );
        y0 = std::max( std::min( y0, img_height - 1.0f ), 0.f );
        x1 = std::max( std::min( x1, img_width - 1.0f ), 0.f );
        y1 = std::max( std::min( y1, img_height - 1.0f  ), 0.f );

        auto score = obj.prob;
        obj.rect.x = x0;
        obj.rect.y = y0;
        obj.rect.width = x1 - x0;
        obj.rect.height = y1 - y0;

        if ( ( obj.rect.width < 48.f ) || ( ( obj.rect.height / obj.rect.width ) > 7.f ) )
        {
            continue;
        }

        BBox current_bbox( img_width, img_height );
        current_bbox.addTlhw( obj.rect.x, obj.rect.y, obj.rect.width, obj.rect.height );
        before_filtered.emplace_back( current_bbox , score ); // ???
    }

    // filter box bottom threshold y
    std::vector<std::pair<BBox,float>> after_filtered;
    for ( const auto& d : before_filtered )
    {
        if ( d.first.ymax > param->bboxBottomThresholdY )
        {
            after_filtered.push_back( d );
        }
    }

    // sort boxes with area descending order
    std::sort( after_filtered.begin(), after_filtered.end(),
               []( const std::pair<BBox,float> &a, const std::pair<BBox,float> &b )
               {
                   return a.first.width * a.first.height > b.first.width * b.first.height;
               } );

    // limit the maximum detections
    for ( unsigned int i = 0;
                    ( i < after_filtered.size() ) && ( i < param->numMaxPerson ); i++ )
    {
        // TODO: implement to add result into descriptor
        yolov4Descriptor->addPerson(after_filtered[i].second, after_filtered[i].first);
    }
    free(outConcatTensor_deq);

    TRACE("postProcessing(): ", sw.getMs(), "ms");
    sw.stop();

    return kAifOk;
}

cv::Mat Yolov4Detector::staticResize(const cv::Mat& img, const int targetWidth, const int targetHeight)
{
    float r = std::min( targetWidth / ( img.cols * 1.0 ), targetHeight / ( img.rows * 1.0 ) );
    int unpad_w = static_cast<int>( r * img.cols );    // 416
    int unpad_h = static_cast<int>( r * img.rows );    // 312

    cv::Mat re( unpad_h, unpad_w, CV_8UC3 ); // 312 x 416
    cv::resize( img, re, re.size() );        // img -> re (312 x 416)
    cv::Mat out( targetHeight, targetWidth, CV_8UC3, cv::Scalar( 114, 114, 114 ) );  // out (416 x 416)
    re.copyTo( out( cv::Rect( 0, 0, re.cols, re.rows ) ) );   // re(312x416) ==> copyTo ==> out (416x416 [312x416])

    return out;
}

t_aif_status Yolov4Detector::getOutputTensorInfo(uint8_t **outTensorDataArr,
                                         size_t &outConcatDimSize)
{
    std::shared_ptr<Yolov4Param> param = std::dynamic_pointer_cast<Yolov4Param>(m_param);
    if (param == nullptr) {
        Loge(__func__, "failed to convert DetectorParam to Yolov4Param");
        return kAifError;
    }

    size_t numAnchor[2];
    unsigned int numCell[2];
    for (auto i = 0; i < param->strides.size(); i++) {
        auto stride = param->strides[i];
        auto nx = static_cast<unsigned int>(m_modelInfo.width / stride);
        auto ny = static_cast<unsigned int>(m_modelInfo.height / stride);
        numAnchor[i] = param->anchors[i].size();
        numCell[i] = nx * ny;
    }

    const std::vector<int> &outputs = m_interpreter->outputs();
    if (outputs.size() != 2) {
        Loge(__func__, " output tensor should be 2!!");
        return kAifError;
    }

    // get dequantization info from outTensor
    for (int i = 0; i < outputs.size(); i++) {
        TfLiteTensor *output = m_interpreter->tensor(outputs[i]);
        if (output == nullptr) {
            Loge(__func__, " can't get tflite tensor_output!!");
            return kAifError;
        }

        if (output->dims == nullptr || output->dims->size != 2) {
            Loge(__func__, " output tensor->dims is null or dims->size is not 2");
            return kAifError;
        }

        size_t outDimSize = 1;
        for (int i = 0; i < output->dims->size; i++) {
            outDimSize = outDimSize * output->dims->data[i];
        }

        int index;
        if (outDimSize == (numAnchor[0] * numCell[0] * param->numOutChannels)) { /* 507 x 85 */
            index = RESULT_YOLO_IDX_0;
        } else if (outDimSize == (numAnchor[1] * numCell[1] * param->numOutChannels)) { /* 2028 * 85 */
            index = RESULT_YOLO_IDX_1;
        } else {
            Loge(__func__, " output dims size is wrong!");
            return kAifError;
        }

        outConcatDimSize += outDimSize;

        TfLiteAffineQuantization* q_params = reinterpret_cast<TfLiteAffineQuantization*>(output->quantization.params);
        if (!q_params) {
            Loge(__func__, " output tensor doesn't have q_params...");
            return kAifError;
        }

        if (q_params->scale->size != 1) {
            Loge(__func__, " output tensor should not per-axis quant...");
            return kAifError;
        }

        outTensorDataArr[index] = reinterpret_cast<uint8_t*>(output->data.uint8);
        mScaleOut[index] = q_params->scale->data[0];
        mZeropointOut[index] = q_params->zero_point->data[0];

        TRACE( __func__, " mScaleOut[" , index , "]: " , mScaleOut[index] , " mZeropointOut[" , index , "]: " , mZeropointOut[index]);
    }

    return kAifOk;
}

t_aif_status Yolov4Detector::transformToImageCoordinates(uint8_t **outTensorDataArr, float *target_outTensor_deq,
                                                 std::shared_ptr<Yolov4Param> &param)
{
    //Logv(__func__, " target_outTensor_deq addr: 0x", target_outTensor_deq);
    const std::vector<int> strides = param->strides;
    const unsigned int numOutChannels = param->numOutChannels;
    const auto num_layers = strides.size();
    if (num_layers == 0) {
        Loge(__func__, " invalid strides: num_layers is 0!");
        return kAifError;
    }

    /* TODO: openmp parallel */
    for ( auto i = 0U; i < num_layers; i++ )    // x 2
    {
        int pos1 = 0;
        for (auto i_p = 0U; i_p < i; i_p++)
        {
            /* previous info at i == 0 */
            auto stride_p = strides[i_p];
            auto nx_p = static_cast<unsigned int>( m_modelInfo.width / stride_p );
            auto ny_p = static_cast<unsigned int>( m_modelInfo.height / stride_p );
            auto numAnchor_p = param->anchors[i_p].size();
            auto numCell_p = nx_p * ny_p;

            pos1 += numAnchor_p * numCell_p * numOutChannels;
        }

        auto stride = strides[i];
        auto nx = static_cast<unsigned int>( m_modelInfo.width / stride );
        auto ny = static_cast<unsigned int>( m_modelInfo.height / stride );
        auto numAnchor = param->anchors[i].size();
        auto numCell = nx * ny;
        uint8_t *outTensorData = outTensorDataArr[i];

        auto DEQ = [this, i](uint8_t data) {
            return (mScaleOut[i] * (data - mZeropointOut[i]));
        };

        for ( auto j = 0U; j < numAnchor; j++ )    // x 3
        {
            const int pos2 = pos1 + j * numCell * numOutChannels;
            auto& anchor = param->anchorsNorm[i][j];


            for ( auto k = 0U; k < numCell; k++ )    // ( x 169 / x 676 )
            {
                const int pos3 = pos2 + k * numOutChannels;
                const int pos3_org = pos3 - pos1;
                const int gridx = k % nx;
                const int gridy = k / ny;

                // dequantization....
                // x
                target_outTensor_deq[pos3 + 0] = ( sigmoid(DEQ(outTensorData[pos3_org + 0])) * 2 - 0.5 + gridx ) * stride;
                // y
                target_outTensor_deq[pos3 + 1] = ( sigmoid(DEQ(outTensorData[pos3_org + 1])) * 2 - 0.5 + gridy ) * stride;
                // w
                target_outTensor_deq[pos3 + 2] = ( pow( sigmoid(DEQ(outTensorData[pos3_org + 2])) * 2, 2 ) * anchor.first ) * stride;
                // h
                target_outTensor_deq[pos3 + 3] = ( pow( sigmoid(DEQ(outTensorData[pos3_org + 3])) * 2, 2 ) * anchor.second ) * stride;
                // objectness and class score
                for ( auto l = 4U; l < numOutChannels; l++ )
                {
                    target_outTensor_deq[pos3 + l] = sigmoid(DEQ(outTensorData[pos3_org + l]));
                }
            }
        }
        pos1 += numAnchor * numCell * numOutChannels;
    }

    return kAifOk;
}


void Yolov4Detector::generateYolov4Proposals( std::vector<Object>& objects , std::shared_ptr<Yolov4Param> &param,
                                              float *outConcatData, size_t outConcatDimSize)
{
    const unsigned int numOutChannels = param->numOutChannels;
    const unsigned int numClasses = param->numClasses;
    size_t object_size = 0;
    auto num_cells = outConcatDimSize / numOutChannels;
    // WARNING: If mTargetClasses.size() > 1 (not Person class only), objects can exceed num_cells
    objects.resize( num_cells );

    TRACE(__func__, " num_cells: ", num_cells, " numOutChannels: ", numOutChannels);

    /* TODO: #pragma omp parallel for */
    for ( auto i = 0U; i < num_cells; i++ )
    {
        const unsigned int basic_pos = CHECK_UINT_MUL(i, numOutChannels); //i * numOutChannels;

        float x_center = outConcatData[basic_pos + 0];
        float y_center = outConcatData[basic_pos + 1];
        float w = outConcatData[basic_pos + 2];
        float h = outConcatData[basic_pos + 3];
        float box_objectness = outConcatData[basic_pos + 4];

        float x0 = x_center - w * 0.5f;
        float y0 = y_center - h * 0.5f;

        if ( box_objectness > param->bbox_conf_threshold )
        {
            for ( auto class_idx = 0U; class_idx < numClasses; class_idx++ )
            {
                float box_cls_score = outConcatData[basic_pos + 5 + class_idx];
                float box_prob = box_objectness * box_cls_score;
                /*auto it_b = std::begin( mTargetClasses );
                auto it_e = std::end( mTargetClasses );*/
                if ( class_idx == 0 && box_prob > param->bbox_conf_threshold) // && ( std::find( it_b, it_e, class_idx ) != it_e ) )
                {
                    size_t obj_idx;
                    /*TODO: #pragma omp atomic capture*/
                    obj_idx = object_size++;

                    Object &obj = objects[obj_idx];
                    obj.rect.x = x0;
                    obj.rect.y = y0;
                    obj.rect.width = w;
                    obj.rect.height = h;
                    obj.label = class_idx;
                    obj.prob = box_prob;
                }
            }
        }
    } // point anchor loop

    objects.resize( object_size );
}

void
Yolov4Detector::qsortDescentInplace( std::vector<Object>& objects )
{
    if ( objects.empty() == false )
    {
        qsortDescentInplace( objects, 0, objects.size() - 1 );
    }
}

void
Yolov4Detector::qsortDescentInplace( std::vector<Object>& objects, int left, int right )
{
    int i = left;
    int j = right;
    float p = objects[( left + right ) / 2].prob;

    while ( i <= j )
    {
        while ( objects[i].prob > p )
        {
            i++;
        }

        while ( objects[j].prob < p )
        {
            j--;
        }

        if ( i <= j )
        {
            // swap
            std::swap( objects[i], objects[j] );

            i++;
            j--;
        }
    }

    //#pragma omp parallel sections
    {
        //#pragma omp section
        {
            if ( left < j )
            {
                qsortDescentInplace( objects, left, j );
            }
        }
        //#pragma omp section
        {
            if ( i < right )
            {
                qsortDescentInplace( objects, i, right );
            }
        }
    }
}

void
Yolov4Detector::nmsSortedBboxes(std::shared_ptr<Yolov4Param> &param,  const std::vector<Object>& objects, std::vector<unsigned int>& picked )
{
    picked.clear();

    const size_t n = objects.size();
    TRACE(__func__, " n: ", n);

    std::vector<float> areas( n );
    for ( int i = 0; i < n; i++ )
    {
        areas[i] = objects[i].rect.area();
    }

    for ( int i = 0; i < n; i++ )
    {
        const auto& a = objects[i];

        int keep = 1;
        for ( int j = 0; j < static_cast<int>( picked.size() ); j++ )
        {
            const auto& b = objects[picked[j]];

            // intersection over union
            const float inter_area = intersectionArea( a, b );
            const float union_area = areas[i] + areas[picked[j]] - inter_area;
            if ( ( inter_area / union_area ) >  param->nms_threshold)
            {
                keep = 0;
            }
        }

        if ( keep )
        {
            picked.push_back( i );
        }
    }
}



} // end of namespace aif
