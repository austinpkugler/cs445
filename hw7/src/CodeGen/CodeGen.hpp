#pragma once

// #include "Instruction.hpp"
#include "EmitCode/EmitCode.hpp"
#include "../Tree/Tree.hpp"
#include "../Semantics/Semantics.hpp"

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

class CodeGen
{
    public:
        CodeGen(const Node *root, const std::string tmPath, Semantics *analyzer);
        ~CodeGen();

        // Helpers
        void generate();

    private:
        // Print
        void printFuncs() const;

        // Generate
        void generateAndTraverse(const Node *node);
        void generateEnd(const Node *node);
        void generateDecl(Decl *decl);
        void generateExp(const Exp *exp);
        void generateBinary(const Binary *binary);
        void generateConst(const Const *constN);
        void generateStmt(const Stmt *stmt);

        // Emit
        void emitIO();

        // Helpers
        char * toChar(const std::string comment) const;

        const Node *m_root;
        const std::string m_tmPath;
        Semantics *m_analyzer;
        int m_toffset;
        int m_goffset;
        std::map<std::string, int> m_funcs;
        std::vector<Var *> m_globals;
};
