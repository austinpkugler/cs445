#include "Compound.hpp"

Compound::Compound(const int lineNum) : Stmt::Stmt(lineNum) {}

std::string Compound::stringify() const
{
    return "Compound";
}
