#ifndef AIF_XNNPACKDELEGATE_H
#define AIF_XNNPACKDELEGATE_H

#include <tensorflow/lite/delegates/xnnpack/xnnpack_delegate.h>
#include <aif/base/Types.h>
#include <string>
#include <vector>
#include <memory>

namespace aif {

using XnnpackDelegatePtr = std::unique_ptr<TfLiteDelegate, decltype(&TfLiteXNNPackDelegateDelete)>;
class XnnpackDelegate {
public:
    XnnpackDelegate();
    virtual ~XnnpackDelegate();

    t_aif_status setXnnpackDelegateOptions(
                            const std::string& options,
                            bool& useXnnpack,
                            int& numThreads);
    XnnpackDelegatePtr createXnnpackDelegate();

private:
    TfLiteXNNPackDelegateOptions m_delegateOptions;
};

} // end of namespace aif

#endif // AIF_XNNPACKDELEGATE_H
