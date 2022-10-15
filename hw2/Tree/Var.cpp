#include "Var.hpp"

Var::Var(const unsigned tokenLineNum, const Primitive type, const std::string value, const bool isStatic) : Node::Node(tokenLineNum, value), m_type(type), m_isStatic(isStatic)
{

}

void Var::setType(const Primitive::Type type)
{
    if (m_sibling != nullptr)
    {
        Var *node = dynamic_cast<Var *>(m_sibling);
        node->setType(type);
    }
}

void Var::makeStatic()
{
    if (m_sibling != nullptr)
    {
        ((Var* )m_sibling)->makeStatic();
    }
    m_isStatic = true;
}

void Var::printNode() const
{
    std::cout << stringify();
    m_type.printNode();
}

std::string Var::stringify() const
{
    return "Var" + m_stringValue + " ";
}
