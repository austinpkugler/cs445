#pragma once

// #include "Instruction.hpp"
#include "EmitCode/EmitCode.hpp"
#include "../Tree/Tree.hpp"
#include "../Semantics/Is.hpp"

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

class CodeGen
{
    public:
        CodeGen(const Node *root, const std::string cMinusPath, const std::string tmPath);
        ~CodeGen();

        // Helpers
        void generate();

    private:
        // Print
        void printFuncs() const;

        // Generate
        void generateDecl(Decl *decl);
        void generateExp(const Exp *exp);
        void generateStmt(const Stmt *stmt);

        // Emit
        void emitAndTraverse(const Node *node);
        void emitEnd(const Node *node);
        void emitIO();

        // Helpers
        char * toChar(const std::string comment) const;

        const Node *m_root;
        const std::string m_cMinusPath;
        const std::string m_tmPath;
        int m_toffset;
        int m_goffset;
        std::map<std::string, int> m_funcs;
        std::vector<Var *> m_globals;
};
