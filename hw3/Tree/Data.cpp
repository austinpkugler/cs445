#include "Data.hpp"

Data::Data(Data::Type type, bool isArray, bool isStatic) : m_isArray(isArray), m_isStatic(isStatic)
{
    m_next = nullptr;
    if (isArray)
    {
        m_next = new Data(type, false, false);
        m_type = Data::Type::Void;
    }
    else
    {
        m_type = type;
    }
}

void Data::setIsArray(bool isArray)
{
    m_isArray = isArray;
}

void Data::setIsStatic(bool isStatic)
{
    m_isStatic = isStatic;
}

void Data::setType(Data::Type type)
{
    m_type = type;
    if (m_next != nullptr)
    {
        m_next->setType(type);
    }
}

std::string Data::stringify() const
{
    std::string stringy;
    switch(m_type)
    {
        case Data::Type::Int:
            stringy = "int";
            break;
        case Data::Type::Bool:
            stringy = "bool";
            break;
        case Data::Type::Char:
            stringy = "char";
            break;
        case Data::Type::String:
            stringy = "string";
            break;
        case Data::Type::Void:
            stringy = "void";
            break;
        default:
            throw std::runtime_error("Could not stringify Data::Type");
            break;
    }
    return stringy;
}
