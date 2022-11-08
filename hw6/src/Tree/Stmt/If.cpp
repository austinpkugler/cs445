#include "If.hpp"

If::If(const int lineNum) : Stmt::Stmt(lineNum) {}

std::string If::stringify() const
{
    return "If";
}
