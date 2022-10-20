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
        // All nodes
        void analyzeTree(Node *node);

        // Decl nodes
        void analyzeDecl(const Decl *decl);
        void analyzeFunc(const Func *func);
        void analyzeParm(const Parm *parm);
        void analyzeVar(Var *var);

        // Exp nodes
        void analyzeExp(Exp *exp);
        void analyzeAsgn(const Asgn *asgn);
        void analyzeBinary(const Binary *binary) const;
        void analyzeCall(const Call *call) const;
        void analyzeId(const Id *id) const;
        void analyzeUnary(const Unary *unary) const;
        void analyzeUnaryAsgn(const UnaryAsgn *unaryAsgn) const;

        // Stmt nodes
        void analyzeStmt(const Stmt *stmt) const;
        void analyzeCompound(const Compound *compound) const;
        void analyzeFor() const;
        void analyzeReturn(const Return *returnN) const;

        // Error and warn checking
        bool isValidMainFunc(const Func *func) const;
        bool isDeclaredId(const Id *id) const;
        void checkSameTypeOperands(Exp *exp) const;
        void checkArray(const Id *arrayId, const Node *indexNode) const;
        void checkOperandTypes(const Exp *exp) const;
        void checkBinaryIntOperands(const Binary *binary) const;
        void checkBinaryBoolOperands(const Binary *binary) const;
        void checkBinaryOperandsAreNotArray(const Binary *binary) const;
        void checkAsgnOperands(const Asgn *asgn) const;
        void checkAsgnIntOperands(const Asgn *asgn) const;
        void checkUnaryOperands(const Unary *unary) const;
        void checkUnaryAsgnOperands(const UnaryAsgn *unaryAsgn) const;

        // Symbol table
        void leaveScope();
        bool addToSymTable(const Decl *decl, const bool global=false);
        Decl * getFromSymTable(const std::string name) const;

        // Helpers
        Data * setAndGetExpData(const Exp *exp) const;
        bool expOperandsExist(const Exp *exp) const;
        bool expIsIndex(const Exp *exp) const;

        SymTable *m_symTable;
        Node *m_root;
        bool m_validMainExists = false;
};
