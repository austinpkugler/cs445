#include "Parm.hpp"

Parm::Parm(const unsigned tokenLineNum, Primitive *type, const std::string value) : Node::Node(tokenLineNum, value), m_type(type)
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

void Parm::printNode() const
{
    std::cout << stringify();
    // m_type.printNode();
}

std::string Parm::stringify() const
{
    return "Parm" + m_stringValue + " ";
}
