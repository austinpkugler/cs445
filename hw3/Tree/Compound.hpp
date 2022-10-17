#pragma once

#include "Node.hpp"

class Compound : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the call occurred in.
         */
        Compound(const unsigned tokenLineNum);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Stmt; }
        StmtKind getStmtKind() const { return StmtKind::Compound; }
        std::string stringify() const;
};
