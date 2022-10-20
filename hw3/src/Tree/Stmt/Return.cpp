#include "Return.hpp"

Return::Return(const unsigned lineNum) : Stmt::Stmt(lineNum, Stmt::Kind::Return)
{

}

std::string Return::stringify() const
{
    return "Return";
}
