#pragma once

#include "Tree/Tree.hpp"
#include "SymTable.hpp"

class Semantics
{
    public:
        Semantics(SymTable *symTable);

        void analyze(Node *node) const;

    private:
        void analyzeDecl(Node *node) const;
        void analyzeStmt(Node *node) const;
        void analyzeExp(Node *node) const;

        SymTable *m_symTable;
        Node *m_root;
        bool m_mainExists = false;
};
