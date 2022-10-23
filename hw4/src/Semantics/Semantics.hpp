#pragma once

#include "Is.hpp"
#include "SymTable.hpp"
#include "../Emit/Emit.hpp"
#include "../Tree/Tree.hpp"

#include <algorithm>
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
        void analyzeFunc(const Func *func);
        void analyzeParm(const Parm *parm);
        void analyzeVar(Var *var);
        void analyzeAsgn(const Asgn *asgn);
        void analyzeBinary(const Binary *binary) const;
        void analyzeCall(const Call *call) const;
        void analyzeId(const Id *id) const;
        void analyzeUnary(const Unary *unary) const;
        void analyzeUnaryAsgn(const UnaryAsgn *unaryAsgn) const;
        void analyzeBreak(const Break *breakN) const;
        void analyzeIf(const If *ifN) const;
        void analyzeRange(const Range *range) const;
        void analyzeReturn(const Return *returnN) const;

        // Checks
        void checkOperandsOfSameType(Exp *exp) const;
        void checkOperandsOfType(Exp *exp, const Data::Type type) const;
        void checkIndex(const Binary *binary) const;
        void checkUnusedWarns() const;

        // Symbol table
        bool symTableInsert(const Decl *decl, const bool global=false, const bool showWarns=true);
        Decl * symTableGet(const std::string name) const;
        void symTableInitialize(Node *node);
        Data * symTableSetType(Node *node);
        void symTableSimpleEnterScope(const std::string name);
        void symTableSimpleLeaveScope(const bool showWarns=false);
        void symTableEnterScope(const Node *node);
        void symTableLeaveScope(const Node *node, const bool showWarns=true);

        // Helpers
        bool isMainFunc(const Func *func) const;
        bool expOperandsExist(const Exp *exp) const;
        bool lhsExists(const Exp *exp) const;
        std::string getExpSym(const Exp *exp) const;
        bool hasIndexAncestor(const Exp *exp) const;
        bool hasAsgnAncestor(const Exp *exp) const;

        SymTable *m_symTable;
        bool m_mainExists;
};
