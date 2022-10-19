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

        void analyze(const Node *node);

    private:
        // All nodes
        void analyzeTree(const Node *node);

        // Decl nodes
        void analyzeDecl(const Decl *decl);
        void analyzeFunc(const Func *func);
        void analyzeParm(const Parm *parm);
        void analyzeVar(Var *var);

        bool isDecl(const Node *node) const;
        bool isFunc(const Node *node) const;
        bool isParm(const Node *node) const;
        bool isVar(const Node *node) const;

        // Exp nodes
        void analyzeExp(Exp *exp) const;
        void analyzeAsgn(const Asgn *asgn) const;
        void analyzeBinary(const Binary *binary) const;
        void analyzeCall(const Call *call) const;
        void analyzeId(const Id *id) const;
        void analyzeUnary(const Unary *unary) const;
        void analyzeUnaryAsgn(const UnaryAsgn *unaryAsgn) const;

        bool isExp(const Node *node) const;
        bool isAsgn(const Node *node) const;
        bool isBinary(const Node *node) const;
        bool isCall(const Node *node) const;
        bool isConst(const Node *node) const;
        bool isId(const Node *node) const;
        bool isUnary(const Node *node) const;
        bool isUnaryAsgn(const Node *node) const;

        // Stmt nodes
        void analyzeStmt(const Stmt *stmt) const;
        void analyzeCompound(const Compound *compound) const;
        void analyzeFor() const;
        void analyzeReturn(const Return *returnN) const;

        bool isStmt(const Node *node) const;
        bool isBreak(const Node *node) const;
        bool isCompound(const Node *node) const;
        bool isFor(const Node *node) const;
        bool isIf(const Node *node) const;
        bool isRange(const Node *node) const;
        bool isReturn(const Node *node) const;
        bool isWhile(const Node *node) const;

        // Error and warn checking
        bool isValidMainFunc(const Func *func) const;
        bool isDeclaredId(const Id *id) const;
        void checkArray(const Id *arrayId, const Node *indexNode) const;
        void checkOperandsAreSameType(const Exp *exp) const;
        void checkOperandsAreCorrectType(const Exp *exp) const;

        // Symbol table
        void leaveScope();
        bool addToSymTable(const Decl *decl, const bool global=false);
        Decl * getFromSymTable(const std::string name) const;

        // Helpers
        Data * setAndGetExpData(const Exp *exp) const;
        bool expOperandsExist(const Exp *exp) const;

        SymTable *m_symTable;
        Node *m_root;
        bool m_validMainExists = false;
};
