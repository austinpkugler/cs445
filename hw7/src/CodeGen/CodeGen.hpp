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
        // Generate
        void generateDecl(const Decl *decl);
        void generateExp(const Exp *exp);
        void generateStmt(const Stmt *stmt);

        // Emit
        void emitAndTraverse(const Node *node);
        void emitEnd(const Node *node);
        void emitIO() const;

        // Helpers
        char * toChar(const std::string comment) const;

        const Node *m_root;
        const std::string m_cMinusPath;
        const std::string m_tmPath;
        int m_toff;
        int m_mainLoc;
};
