#include "Instruction.hpp"

Instruction::Instruction() : m_type(Instruction::Type::None), m_comment("* ** ** ** ** ** ** ** ** ** ** ** **") {}

Instruction::Instruction(const std::string comment) : m_type(Instruction::Type::None), m_comment("* " + comment) {}

Instruction::Instruction(const Instruction::Type type, const std::string comment, const int instructionNum, const std::string reg0, const std::string reg1, const std::string reg2) : m_type(type), m_comment(comment), m_instructionNum(instructionNum), m_reg0(reg0), m_reg1(reg1), m_reg2(reg2) {}

// Instruction Instruction::createLDC(const std::string comment, const unsigned destReg, const std::string value)
// {
//     s_instructionCount++;
//     return Instruction(Instruction::Type::LDC, comment, s_instructionCount - 1, std::to_string(destReg), value, "6");
// }


