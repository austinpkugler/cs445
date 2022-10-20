#include "While.hpp"

While::While(const unsigned lineNum) : Stmt::Stmt(lineNum, Stmt::Kind::While)
{

}

std::string While::stringify() const
{
    return "While";
}
