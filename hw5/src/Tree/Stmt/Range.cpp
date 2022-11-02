#include "Range.hpp"

Range::Range(const int lineNum) : Stmt::Stmt(lineNum) {}

std::string Range::stringify() const
{
    return "Range";
}
