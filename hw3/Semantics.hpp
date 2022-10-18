#pragma once

#include "Tree/Tree.hpp"
#include "SymTable.hpp"
#include "Emit.hpp"

class Semantics
{
    public:
        /**
         * @param symTable Symbol table instance to use during semantic analysis.
         */
        Semantics(SymTable *symTable);

        void analyze(Node *node);

    private:
        void analyzeTree(Node *node);
        void analyzeDecl(Node *node);
        void analyzeStmt(Node *node) const;
        void analyzeExp(Node *node) const;

        // bool isValidIdNode(Node *node) const;
        // bool isValidFuncNode(Node *node) const;

        bool isValidMainFunc(Func *funcNode) const;
        bool isDeclaredId(Id *idNode) const;

        SymTable *m_symTable;
        Node *m_root;
        bool m_validMainExists = false;
};
