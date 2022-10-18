#pragma once

#include "../Node.hpp"

class While : public Node
{
    public:
        /**
         * @param tokenLineNum Line number the while occurred in.
         */
        While(const unsigned tokenLineNum);

        // Overridden
        NodeKind getNodeKind() const { return NodeKind::Stmt; }
        StmtKind getStmtKind() const { return StmtKind::While; }
        std::string stringify() const;
};
