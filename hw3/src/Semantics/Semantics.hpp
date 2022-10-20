#pragma once

#include "SymTable.hpp"
#include "Is.hpp"
#include "../Emit/Emit.hpp"
#include "../Tree/Tree.hpp"

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
        // Analyze
        void analyzeTree(Node *node);
        void analyzeDecl(const Decl *decl);
        void analyzeFunc(const Func *func);
        void analyzeParm(const Parm *parm);
        void analyzeVar(Var *var);
        void analyzeExp(Exp *exp);
        void analyzeAsgn(const Asgn *asgn);
        void analyzeBinary(const Binary *binary) const;
        void analyzeCall(const Call *call) const;
        void analyzeId(const Id *id) const;
        void analyzeUnary(const Unary *unary) const;
        void analyzeUnaryAsgn(const UnaryAsgn *unaryAsgn) const;
        void analyzeStmt(const Stmt *stmt) const;
        void analyzeCompound(const Compound *compound) const;
        void analyzeFor() const;
        void analyzeReturn(const Return *returnN) const;

        // Check
        void checkOperandsOfSameType(Exp *exp) const;
        void checkOperandsOfType(Exp *exp, const Data::Type type) const;
        void checkUnaryOperands(const Unary *unary) const;
        void checkUnaryAsgnOperands(const UnaryAsgn *unaryAsgn) const;
        void checkIndex(const Binary *binary) const;

        // Symbol Table
        void leaveScope();
        bool addToSymTable(const Decl *decl, const bool global=false);
        Decl * getFromSymTable(const std::string name) const;

        // Helpers
        bool isMainFunc(const Func *func) const;
        bool isDeclared(const Id *id) const;
        bool hasIndexAncestor(const Exp *exp) const;
        bool hasAsgnAncestor(const Exp *exp) const;
        bool expOperandsExist(const Exp *exp) const;
        bool lhsExists(const Exp *exp) const;
        std::string getExpSym(const Exp *exp) const;
        Data * setAndGetExpData(const Exp *exp) const;

        SymTable *m_symTable;
        Node *m_root;
        bool m_validMainExists = false;
};
