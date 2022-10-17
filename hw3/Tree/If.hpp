#pragma once

#include "Node.hpp"

class If : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the if occurred in.
         */
        If(const unsigned tokenLineNum);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Stmt; }
        StmtKind getStmtKind() const { return StmtKind::If; }
        std::string stringify() const;
};
