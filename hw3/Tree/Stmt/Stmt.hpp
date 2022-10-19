#pragma once

#include "../Node.hpp"

class Stmt : public Node
{
    public:
        // Enums
        enum class Kind { Break, Compound, For, If, Return, While, Range };

        /**
         * @param lineNum Line number of occurrence.
         * @param stmtKind The category of stmt node.
         */
        Stmt(const unsigned lineNum, const Stmt::Kind stmtKind);

        // Getters
        Stmt::Kind getStmtKind() const { return m_stmtKind; }

    private:
        const Stmt::Kind m_stmtKind;
};
