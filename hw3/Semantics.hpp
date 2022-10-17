#pragma once

#include "Tree/Tree.hpp"
#include "SymTable.hpp"

class Semantics
{
    public:
        Semantics(SymTable *symTable);

        void analyze(Node *node);

    private:
        void analyzeDecl(Node *node);
        void analyzeStmt(Node *node) const;
        void analyzeExp(Node *node) const;

        SymTable *m_symTable;
        Node *m_root;
        bool m_mainExists = false;
};
