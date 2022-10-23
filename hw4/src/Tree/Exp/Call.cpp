#include "Call.hpp"

Call::Call(const int lineNum, const std::string funcName) : Exp::Exp(lineNum, new Data(Data::Type::Undefined, false, false)), m_name(funcName) {}

std::string Call::stringify() const
{
    return "Call: " + m_name;
}

unsigned Call::getParmCount() const
{
    return getParms().size();
}

std::vector<Node *> Call::getParms() const
{
    std::vector<Node *> parms;
    Node *currParm = getChild();
    while (currParm != nullptr)
    {
        parms.push_back(currParm);
        currParm = currParm->getSibling();
    }
    return parms;
}
