#include "Range.hpp"

Range::Range(const unsigned lineNum) : Stmt::Stmt(lineNum, Stmt::Kind::Range)
{

}

std::string Range::stringify() const
{
    return "Range";
}
