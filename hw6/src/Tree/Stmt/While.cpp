#include "While.hpp"

While::While(const int lineNum) : Stmt::Stmt(lineNum) {}

std::string While::stringify() const
{
    return "While";
}
