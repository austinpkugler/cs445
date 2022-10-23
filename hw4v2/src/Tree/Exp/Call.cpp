#include "Call.hpp"

Call::Call(const int lineNum, const std::string funcName) : Exp::Exp(lineNum, new Data(Data::Type::Undefined, false, false)), m_name(funcName) {}

std::string Call::stringify() const
{
    return "Call: " + m_name;
}
