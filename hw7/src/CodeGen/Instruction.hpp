#pragma once

#include <string>

class Instruction
{
    public:
        enum class Type { None, LDC, LDA, LD, ST, JNZ, JZR, JMP, HALT, NOP, IN, INB, INC, OUT, OUTB, OUTC, OUTNL, ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, NOT, NEG, SWP, RND, TLT, TLE, TEQ, TNE, TGE, TGT, SLT, SGT, MOV, SET, CO, COA, LIT };

        Instruction();
        Instruction(const std::string comment);
        Instruction(const Instruction::Type type, const std::string comment, const int instructionNum, const std::string reg0, const std::string reg1, const std::string reg2);

        // Static
        // LDC
        static Instruction createInstruction(const Instruction::Type type, const std::string comment, const unsigned reg, const std::string value);
        // LDA
        static Instruction createInstruction(const Instruction::Type type, const std::string comment, const unsigned reg, const unsigned reg2, const int value);


    private:
        // Static
        static unsigned s_instructionCount;

        const Instruction::Type m_type;
        const std::string m_comment;
        int m_instructionNum;
        std::string m_reg0;
        std::string m_reg1;
        std::string m_reg2;
};
