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
        CodeGen(const Node *root, const std::string tmPath);
        ~CodeGen();

        // Helpers
        void generate();

    private:
        // Generate
        void generateAndTraverse(const Node *node);
        void generateEnd(const Node *node);
        void generateNode(const Node *node);
        void generateFunc(const Func *func);
        void generateParm(const Parm *parm);
        void generateVar(Var *var);
        void generateAsgn(const Asgn *asgn);
        void generateBinary(const Binary *binary);
        void generateCall(const Call *call);
        void generateConst(const Const *constN);
        void generateId(const Id *id);
        void generateUnary(const Unary *unary);
        void generateReturn(const Return *returnN);

        // Emit
        void emitIO();

        // Helpers
        char * toChar(const std::string comment) const;
        std::string toUpper(std::string s) const;

        const Node *m_root;
        const std::string m_tmPath;
        int m_toffset;
        int m_goffset;
        std::map<std::string, int> m_funcs;
        std::vector<Var *> m_globals;
};
