#pragma once

#include "Node.hpp"

class Call : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the call occurred in.
         * @param funcName Name of the function called.
         */
        Call(const unsigned tokenLineNum, const std::string funcName);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Exp; }
        ExpKind getExpKind() const { return ExpKind::Call; }
        std::string stringify() const;
};
