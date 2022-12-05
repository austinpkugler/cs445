#pragma once

// #include "Instruction.hpp"
#include "EmitCode/EmitCode.hpp"
#include "../Tree/Tree.hpp"
#include "../Semantics/Is.hpp"

#include <filesystem>

FILE *code = NULL;

class CodeGen
{
    public:
        CodeGen(Node *root);
        ~CodeGen();

        // Helpers
        void generate(const std::string filepath);

    private:
        // Helpers
        void generateFuncDecl(Func *func);
        void emitAndTraverse(Node *node);
        void emitIO() const;
        char * toChar(const std::string comment);

        Node *m_root;
};
