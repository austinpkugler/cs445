#include "Parm.hpp"

Parm::Parm(const unsigned tokenLineNum, const Primitive::Type type, const std::string value) : Node::Node(tokenLineNum, value), m_type(type)
{

}

void Parm::printNode() const
{
    std::cout << stringify();
    m_type.printNode();
}

std::string Parm::stringify() const
{
    return "Parm" + m_stringValue + " ";
}
