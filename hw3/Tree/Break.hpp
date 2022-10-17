#pragma once

#include "Node.hpp"

class Break : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the break occurred in.
         */
        Break(const unsigned tokenLineNum);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Stmt; }
        StmtKind getStmtKind() const { return StmtKind::Break; }
        std::string stringify() const;
};
