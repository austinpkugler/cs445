#pragma once

// #include "Instruction.hpp"
#include "EmitCode/EmitCode.hpp"
#include "../Tree/Tree.hpp"
#include "../Semantics/Semantics.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

class CodeGen
{
    public:
        CodeGen(Node *root, const std::string tmPath, bool showLog=false);
        ~CodeGen();

        // Helpers
        void generate();

    private:
        // Generate
        void sortGlobals();
        void generateGlobals();
        void generateAndTraverse(Node *node, const bool generateGlobals=false);
        void generateNode(Node *node, const bool generateGlobals=false);
        void generateFunc(Func *func);
        void generateParm(Parm *parm);
        void generateVar(Var *var, const bool generateGlobals=false);
        void generateAsgn(Asgn *asgn);
        void generateBinary(Binary *binary);
        void generateBinaryIndex(Binary *binary);
        void generateBinaryIndexValue(Binary *binary, Node *indexValue=nullptr);
        void generateCall(Call *call);
        void generateConst(Const *constN);
        void generateId(Id *id);
        void generateUnary(Unary *unary);
        void generateUnaryAsgn(UnaryAsgn *unaryAsgn);
        void generateBreak(Break *breakN);
        void generateCompound(Compound *compound);
        void generateFor(For *forN);
        void generateIf(If *ifN);
        void generateRange(Range *range);
        void generateReturn(Return *returnN);
        void generateWhile(While *whileN);
        void generateEnd(Node *node);

        // Logging
        void log(const std::string msg, const int lineNum) const;
        void logToffset(const std::string loc, const int lineNum) const;
        void logBreak() const;

        Node *m_root;
        const std::string m_tmPath;
        bool m_showLog;
        bool m_mainHasReturn;
        std::vector<int> m_toffsets;
        int m_goffset;
        int m_litOffset;
        std::map<std::string, int> m_funcs;
        std::vector<Var *> m_globals;
};
