/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NODE_TYPE_H
#define AIF_NODE_TYPE_H

#include <string>
#include <cstring>

namespace aif {

class NodeType {
public:
    enum {
        NONE    = 0x0,
        IMAGE   = (0x01 << 1),
        INFERENCE = (0x01 << 2),
        //TENSOR  = (0x01 << 3)
    };
    NodeType();
    NodeType(int type);
    ~NodeType();

    bool operator==(const NodeType& other) const;
    bool operator!=(const NodeType& other) const;

    static int fromString(const std::string& type);

    bool isContain(int type) const;
    bool isContain(const NodeType& type) const;
    void addType(int type);

    std::string toString() const;


private:
    int getInt() const;

private:
    int m_type;
};

} // end of namespace aif

#endif // AIF_NODE_TYPE_H
