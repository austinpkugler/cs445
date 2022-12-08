#pragma once

#include "Emit.hpp"
#include "Is.hpp"
#include "SymTable.hpp"
#include "../Tree/Tree.hpp"

#include <algorithm>
#include <sstream>
#include <string>

class Semantics
{
    public:
        Semantics(SymTable *symTable, const bool verbose);

        // Static
        static void printGoffset() { std::cout << "Offset for end of global space: " << s_goffset << std::endl; }

        // Getters
        SymTable * getSymTable() const { return m_symTable; }

        // Helpers
        Decl * lookupDecl(Id *id);
        void analyze(Node *node);

    private:
        // Analyze
        void analyzeTree(Node *node);
        void analyzeFunc(Func *func);
        void analyzeParm(Parm *parm);
        void analyzeVar(Var *var);
        void analyzeAsgn(const Asgn *asgn);
        void analyzeBinary(const Binary *binary) const;
        void analyzeCall(const Call *call) const;
        void analyzeConst(Const *constN) const;
        void analyzeId(Id *id) const;
        void analyzeUnary(const Unary *unary) const;
        void analyzeUnaryAsgn(const UnaryAsgn *unaryAsgn) const;
        void analyzeBreak(const Break *breakN) const;
        void analyzeCompound(Compound *compound) const;
        void analyzeFor(For *forN) const;
        void analyzeIf(const If *ifN) const;
        void analyzeRange(const Range *range) const;
        void analyzeReturn(const Return *returnN) const;
        void analyzeWhile(const While *whileN) const;

        // Checks
        void checkOperandsOfSameType(Exp *exp) const;
        void checkOperandsOfType(Exp *exp, const Data::Type type, const bool isMath=true) const;
        void checkIndex(const Binary *binary) const;
        void checkUnusedWarns() const;

        // Symbol table
        bool symTableInsert(const Decl *decl, const bool global=false, const bool showWarns=true);
        Decl * symTableGet(const std::string name) const;
        void symTableInitialize(Node *node);
        Data * symTableSetType(Node *node);
        void symTableSimpleEnterScope(const std::string name);
        void symTableSimpleLeaveScope(const bool showWarns=false);
        bool symTableEnterScope(const Node *node);
        bool symTableLeaveScope(const Node *node, const bool showWarns=true);
        void symTableInitializeIOTree();
        void symTableInjectIOTree(Node *node);

        // Helpers
        bool isMainFunc(const Func *func) const;
        bool expOperandsExist(const Exp *exp) const;
        bool lhsExists(const Exp *exp) const;
        std::string getExpSym(const Exp *exp) const;
        bool hasIndexRelative(const Exp *exp) const;
        bool hasAsgnRelative(const Exp *exp) const;
        bool hasNonConstantRelative(const Exp *exp) const;

        // Static
        inline static int s_goffset;
        inline static std::vector<int> s_foffsets;

        SymTable *m_symTable;
        bool m_mainExists;
        Node *m_ioRoot;
};
