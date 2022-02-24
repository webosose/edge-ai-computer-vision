#include <aif/delegate/XnnpackDelegate.h>
#include <aif/tools/Utils.h>

namespace aif {

XnnpackDelegate::XnnpackDelegate()
    : m_delegateOptions({0})
{
}

XnnpackDelegate::~XnnpackDelegate()
{
}

t_aif_status XnnpackDelegate::setXnnpackDelegateOptions(
                            const std::string& options,
                            bool& useXnnpack,
                            int& numThreads)
{
    if (options.empty()) {
        return kAifOk;
    }

    /* "true;4", "false" */
    std::vector<std::string> opts = aif::splitString(options, ';');
    if (opts.size() > 2) {
        std::cout << __func__ << ": failed. too much options string... " << options << std::endl;
        return kAifError;
    }

    if (!opts[0].compare("true")) {
        useXnnpack = true;
        if (opts.size() < 2) {
            numThreads = 1;
        } else {
            numThreads = stoi(opts[1]);
        }

        std::cout << "useXnnpack, numThread: " << numThreads << std::endl;
        return kAifOk;
    } else if (!opts[0].compare("false")) {
        useXnnpack = false;
        std::cout << "No use Xnnpack\n";
        return kAifOk;
    } else {
        std::cout << __func__ << ": failed. wrong options string... " << options << std::endl;
        return kAifError;
    }

    m_delegateOptions.num_threads = numThreads; 

    return kAifOk;
}

XnnpackDelegatePtr XnnpackDelegate::createXnnpackDelegate()
{
    return XnnpackDelegatePtr(TfLiteXNNPackDelegateCreate(&m_delegateOptions),
                            TfLiteXNNPackDelegateDelete);
}

} // end of namespace aif
