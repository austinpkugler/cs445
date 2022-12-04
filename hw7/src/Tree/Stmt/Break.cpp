#include "Break.hpp"

Break::Break(const int lineNum) : Stmt::Stmt(lineNum) {}

std::string Break::stringify() const
{
    return "Break";
}
