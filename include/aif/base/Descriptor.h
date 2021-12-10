#ifndef AIF_DESCRIPTOR_H
#define AIF_DESCRIPTOR_H

#include <aif/base/Types.h>
#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>

namespace aif {

class Descriptor
{
public:
    Descriptor();
    virtual ~Descriptor();

    virtual void addResponseName(
            const std::string& responseName,
            const std::string id = "none");
    virtual void addReturnCode(int returnCode);
    std::string toStr();

    bool hasMember(const std::string& member) const;

protected:
    rapidjson::Document m_root;
};

} // end of namespace aif

#endif // AIF_DESCRIPTOR_H
