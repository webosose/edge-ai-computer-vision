#include <aif/log/Logger.h>
#include <aif/mname/EdgeTpuMNameDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <tensorflow/lite/kernels/internal/reference/non_max_suppression.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>

#include <sstream>
#include <stdexcept>

namespace aif {

EdgeTpuMNameDetector::EdgeTpuMNameDetector()
    : MNameDetector("/usr/share/aif/model/tpu_model_name") {}

EdgeTpuMNameDetector::~EdgeTpuMNameDetector() {
    // Releases interpreter instance before the EdgeTpuContext is destroyed.
    // the lifetime of EdgeTpuContext should be longer than all associated
    // interpreter instances.
    m_interpreter.reset();
}

t_aif_status EdgeTpuMNameDetector::compileModel(
    tflite::ops::builtin::BuiltinOpResolver &resolver) /* override*/
{
    Logi("Compile Model: EdgeTpuMNameDetector");
    std::stringstream errlog;
    try {
        TfLiteStatus res = kTfLiteError;
        tflite::ops::builtin::BuiltinOpResolver resolver;

        // Sets up the edgetpu_context. available for any 1 TPU device.
        m_edgetpuContext =
            edgetpu::EdgeTpuManager::GetSingleton()->OpenDevice();
        if (m_edgetpuContext == nullptr) {
            throw std::runtime_error("can't get edgetpu context!!");
        }

        // Registers Edge TPU custom op handler with Tflite resolver.
        resolver.AddCustom(edgetpu::kCustomOp, edgetpu::RegisterCustomOp());
        res = tflite::InterpreterBuilder(*m_model.get(),
                                         resolver)(&m_interpreter);
        if (res != kTfLiteOk || m_interpreter == nullptr) {
            throw std::runtime_error(
                "cpu_model_name interpreter build failed!!");
        }

        m_interpreter->SetExternalContext(kTfLiteEdgeTpuContext,
                                          m_edgetpuContext.get());
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
