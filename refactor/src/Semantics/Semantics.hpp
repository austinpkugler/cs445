#pragma once

#include "Is.hpp"
#include "SymTable.hpp"
#include "../Emit/Emit.hpp"
#include "../Tree/Tree.hpp"

#include <sstream>
#include <string>

class Semantics
{
    public:
        Semantics(SymTable *symTable);

        void analyze(Node *node);

    private:
        // Analyze
        void analyzeTree(Node *node);

        // Symbol table
        bool symTableInsert(const Decl *decl, const bool global=false);
        Decl * symTableGet(const std::string name) const;
        void symTableInitialize(Node *node);
        Data * symTableSetType(Node *node);
        void symTableSimpleEnterScope(const std::string name);
        void symTableSimpleLeaveScope();
        void symTableEnterScope(const Node *node);
        void symTableLeaveScope(const Node *node);

        SymTable *m_symTable;
        bool m_mainExists;
};
