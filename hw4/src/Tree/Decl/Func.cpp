#include "Func.hpp"

Func::Func(const int lineNum, const std::string funcName, Data *data) : Decl::Decl(lineNum, funcName, data) {}

std::string Func::stringify() const
{
    return "Func: " + m_name + " returns type " + m_data->stringify();
}

unsigned Func::getParmCount() const
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
