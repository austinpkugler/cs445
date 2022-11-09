#include "Var.hpp"

Var::Var(const unsigned tokenLineNum, Primitive *type, const std::string varName, const bool isStatic) : Node::Node(tokenLineNum, varName), m_type(type), m_isStatic(isStatic)
{

}

void Var::setType(const Primitive::Type type)
{
    m_type->setType(type);
    if (m_sibling != nullptr)
    {
        Var *node = (Var *)m_sibling;
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

std::string Var::stringify() const
{
    if (m_type->getIsArray() && m_isStatic)
    {
        return "Var: " + m_stringValue + " of static array of type " + m_type->stringify();
    }
    else if (m_type->getIsArray())
    {
        return "Var: " + m_stringValue + " of array of type " + m_type->stringify();
    }
    else if (m_isStatic)
    {
        return "Var: " + m_stringValue + " of static type " + m_type->stringify();
    }
    return "Var: " + m_stringValue + " of type " + m_type->stringify();
}
