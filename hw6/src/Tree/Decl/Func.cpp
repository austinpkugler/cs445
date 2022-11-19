#include "Func.hpp"

Func::Func(const int lineNum, const std::string funcName, Data *data) : Decl::Decl(lineNum, funcName, data), m_hasReturn(false) {}

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

int Func::getSize() const
{
    int size = -2;

    std::vector<Node *> children = getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        if (children[i] == nullptr || children[i]->getNodeKind() != Node::Kind::Var)
        {
            continue;
        }

        Node *child = children[i];
        while (child != nullptr)
        {
            size -= child->getMemSize();
            child = child->getSibling();
        }
    }

    return size;
}
