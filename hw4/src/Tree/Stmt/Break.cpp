#include "Break.hpp"

Break::Break(const unsigned lineNum) : Stmt::Stmt(lineNum, Stmt::Kind::Break)
{

}

std::string Break::stringify() const
{
    return "Break";
}
