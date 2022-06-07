#include <aif/log/Logger.h>
#include <aif/mname/CpuMNameDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <tensorflow/lite/kernels/internal/reference/non_max_suppression.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuMNameDetector::CpuMNameDetector()
    : MNameDetector("/usr/share/aif/model/cpu_model_name") {}

CpuMNameDetector::~CpuMNameDetector() {}

t_aif_status CpuMNameDetector::compileModel(
    tflite::ops::builtin::BuiltinOpResolver &resolver) /* override*/
{
    Logi("Compile Model: CpuMNameDetector");
    std::stringstream errlog;
    try {
        TfLiteStatus res = kTfLiteError;
        if (!m_param->getUseXnnpack()) {
            Logi("Not use xnnpack: BuiltinOpResolverWithoutDefaultDelegates");
            tflite::ops::builtin::BuiltinOpResolverWithoutDefaultDelegates
                resolver;
            res = tflite::InterpreterBuilder(*m_model.get(),
                                             resolver)(&m_interpreter);
        } else {
            Logi("Use xnnpack: BuiltinOpResolver");
            tflite::ops::builtin::BuiltinOpResolver resolver;
            res = tflite::InterpreterBuilder(*m_model.get(),
                                             resolver)(&m_interpreter);
        }
        if (res != kTfLiteOk || m_interpreter == nullptr) {
            throw std::runtime_error(
                "cpu_model_name interpreter build failed!!");
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

} // namespace aif
