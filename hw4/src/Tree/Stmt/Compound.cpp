#include "Compound.hpp"

Compound::Compound(const unsigned lineNum) : Stmt::Stmt(lineNum, Stmt::Kind::Compound)
{

}

std::string Compound::stringify() const
{
    return "Compound";
}
