#pragma once

#include "../Node.hpp"

class For : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the for occurred in.
         */
        For(const unsigned tokenLineNum);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Stmt; }
        StmtKind getStmtKind() const { return StmtKind::For; }
        std::string stringify() const;
};
