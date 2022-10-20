#include "Call.hpp"

Call::Call(const unsigned lineNum, const std::string funcName) : Exp::Exp(lineNum, Exp::Kind::Call, new Data(Data::Type::Undefined, false, false)), m_name(funcName)
{

}

std::string Call::stringify() const
{
    return "Call: " + m_name;
}
