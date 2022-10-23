#include "Call.hpp"

Call::Call(const int lineNum, const std::string funcName) : Exp::Exp(lineNum, new Data(Data::Type::Undefined, false, false)), m_name(funcName) {}

std::string Call::stringify() const
{
    return "Call: " + m_name;
}

unsigned Call::getParmCount() const
{
    int parmCount = 0;
    Node *currParm = getChild();
    while (currParm != nullptr)
    {
        parmCount += 1;
        currParm = currParm->getSibling();
    }
    return parmCount;
}
