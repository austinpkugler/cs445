#include "Prim.hpp"

Prim::Prim(Type type, bool isArray, bool isStatic) : m_isArray(isArray), m_isStatic(isStatic)
{
    m_next = nullptr;
    if (isArray)
    {
        m_next = new Prim(type, false, false);
        m_type = Type::Void;
    }
    else
    {
        m_type = type;
    }
}

void Prim::setIsArray(bool isArray)
{
    m_isArray = isArray;
}

void Prim::setIsStatic(bool isStatic)
{
    m_isStatic = isStatic;
}

void Prim::setType(Type type)
{
    m_type = type;
    if (m_next != nullptr)
    {
        m_next->setType(type);
    }
}

std::string Prim::stringify() const
{
    std::string stringy;
    switch(m_type)
    {
        case Type::Int:
            stringy = "int";
            break;
        case Type::Bool:
            stringy = "bool";
            break;
        case Type::Char:
            stringy = "char";
            break;
        case Type::String:
            stringy = "string";
            break;
        case Type::Void:
            stringy = "void";
            break;
        default:
            throw std::runtime_error("Could not stringify Prim::Type");
            break;
    }
    return stringy;
}
