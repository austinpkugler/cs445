#include "Parm.hpp"

Parm::Parm(const unsigned tokenLineNum, Prim *type, const std::string parmName) : Node::Node(tokenLineNum, parmName), m_type(type)
{

}

std::string Parm::stringify() const
{
    if (m_type->getIsArray())
    {
        return "Parm: " + m_stringValue + " is array of type " + m_type->stringify();
    }
    return "Parm: " + m_stringValue + " of type " + m_type->stringify();
}

void Parm::setType(Prim::Type type)
{
    m_type->setType(type);
    if (m_sibling != nullptr)
    {
        Parm *node = (Parm *)m_sibling;
        node->setType(type);
    }
}

