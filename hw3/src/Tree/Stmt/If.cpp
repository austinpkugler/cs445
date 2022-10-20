#include "If.hpp"

If::If(const unsigned lineNum) : Stmt::Stmt(lineNum, Stmt::Kind::If)
{

}

std::string If::stringify() const
{
    return "If";
}
