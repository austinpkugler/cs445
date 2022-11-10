#include "Func.hpp"

Func::Func(const int lineNum, const std::string funcName, Data *data) : Decl::Decl(lineNum, funcName, data), m_hasReturn(false)
{
    setHasMem(true);
}

std::string Func::stringify() const
{
    return "Func: " + m_name + " returns type " + m_data->stringify();
}

unsigned Func::getParmCount() const
{
    return getParms().size();
}

std::vector<Node *> Func::getParms() const
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
