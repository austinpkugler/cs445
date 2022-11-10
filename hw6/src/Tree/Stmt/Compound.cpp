#include "Compound.hpp"

Compound::Compound(const int lineNum) : Stmt::Stmt(lineNum)
{
    setHasMem(true);
}

std::string Compound::stringify() const
{
    return "Compound";
}
