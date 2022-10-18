#include "Range.hpp"

Range::Range(const unsigned lineNum) : Exp::Exp(lineNum, Exp::Kind::Range)
{

}

std::string Range::stringify() const
{
    return "Range";
}
