#include "Return.hpp"

Return::Return(const int lineNum) : Stmt::Stmt(lineNum) {}

std::string Return::stringify() const
{
    return "Return";
}
