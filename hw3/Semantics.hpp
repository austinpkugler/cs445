#pragma once

#include "Tree/Tree.hpp"
#include "SymTable.hpp"
#include "Emit.hpp"

#include <string>
#include <sstream>

class Semantics
{
    public:
        /**
         * @param symTable Symbol table instance to use during semantic analysis.
         */
        Semantics(SymTable *symTable);

        void analyze(Node *node);

    private:
        // Analysis
        void analyzeTree(Node *node);
        void analyzeDecl(Node *node);
        void analyzeStmt(Node *node) const;
        void analyzeExp(Node *node) const;

        // Scope
        void leaveScope();

        // Symbol table
        bool addToSymTable(const Node *node, const bool global=false);

        // Determining node status
        bool isFuncNode(Node *node) const;
        bool isForNode(Node *node) const;
        bool isIdNode(Node *node) const;
        bool isValidMainFunc(Func *funcNode) const;
        bool isDeclaredId(Id *idNode) const;

        SymTable *m_symTable;
        Node *m_root;
        bool m_validMainExists = false;
};
