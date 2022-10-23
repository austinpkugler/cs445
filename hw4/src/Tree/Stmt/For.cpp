#include "For.hpp"

For::For(const int lineNum) : Stmt::Stmt(lineNum) {}

std::string For::stringify() const
{
    return "For";
}
