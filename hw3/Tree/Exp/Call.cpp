#include "Call.hpp"

Call::Call(const unsigned lineNum, const std::string isFuncame) : Exp::Exp(lineNum, Exp::Kind::Call, new Data(Data::Type::None, false, false)), m_name(isFuncame)
{

}

std::string Call::stringify() const
{
    return "Call: " + m_name;
}
