#pragma once

// #include "Instruction.hpp"
#include "EmitCode/EmitCode.hpp"
#include "../Tree/Tree.hpp"
#include "../Semantics/Is.hpp"

class CodeGen
{
    public:
        CodeGen(const Node *root, const std::string cMinusPath, const std::string tmPath);
        ~CodeGen();

        // Helpers
        void generate();

    private:
        // Helpers
        void generateFuncDecl(const Func *func);
        void generateCompoundStmt(const Compound *compound);
        void emitAndTraverse(const Node *node);
        void emitIO() const;
        char * toChar(const std::string comment) const;

        const Node *m_root;
        const std::string m_cMinusPath;
        const std::string m_tmPath;
};
