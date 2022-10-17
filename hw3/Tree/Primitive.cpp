#include "Primitive.hpp"

Primitive::Primitive(Type type, const bool isArray) : m_isArray(isArray)
{
    m_next = nullptr;
    if (isArray)
    {
        m_next = new Primitive(type);
        m_type = Type::Void;
    }
    else
    {
        m_type = type;
    }
}

std::string Primitive::stringify() const
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
            throw std::runtime_error("Could not stringify Primitive::Type");
            break;
    }
    return stringy;
}

void Primitive::setType(Type type)
{
    m_type = type;
    if (m_next != nullptr)
    {
        m_next->setType(type);
    }
}
