#include "Parm.hpp"

Parm::Parm(const unsigned tokenLineNum, Primitive *type, const std::string parmName) : Node::Node(tokenLineNum, parmName), m_type(type)
{

}

void Parm::setType(Primitive::Type type)
{
    m_type->setType(type);
    if (m_sibling != nullptr)
    {
        Parm *node = (Parm *)m_sibling;
        node->setType(type);
    }
}

std::string Parm::stringify() const
{
    if (m_type->getIsArray())
    {
        return "Parm: " + m_stringValue + " of array of type " + m_type->stringify();
    }
    return "Parm: " + m_stringValue + " of type " + m_type->stringify();
}
