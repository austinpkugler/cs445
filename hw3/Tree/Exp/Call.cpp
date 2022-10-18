#include "Call.hpp"

Call::Call(const unsigned lineNum, const std::string funcName) : Exp::Exp(lineNum, Exp::Kind::Call), m_name(funcName)
{

}

std::string Call::stringify() const
{
    return "Call: " + m_name;
}
