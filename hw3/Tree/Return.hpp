#pragma once

#include "Node.hpp"

class Return : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the return occurred in.
         */
        Return(const unsigned tokenLineNum);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Stmt; }
        StmtKind getStmtKind() const { return StmtKind::Return; }
        std::string stringify() const;
};
