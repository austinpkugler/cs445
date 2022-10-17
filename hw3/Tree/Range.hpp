#pragma once

#include "Node.hpp"

class Range : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the range occurred in.
         */
        Range(const unsigned tokenLineNum);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Exp; }
        ExpKind getExpKind() const { return ExpKind::Range; }
        std::string stringify() const;
};
