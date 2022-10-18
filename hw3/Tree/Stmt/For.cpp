#include "For.hpp"

For::For(const unsigned lineNum) : Stmt::Stmt(lineNum, Stmt::Kind::For)
{

}

std::string For::stringify() const
{
    return "For";
}
