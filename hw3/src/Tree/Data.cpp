#include "Data.hpp"

Data::Data(Data::Type type, bool isArray, bool isStatic) : m_isArray(isArray), m_isStatic(isStatic)
{
    m_next = nullptr;
    if (isArray)
    {
        m_next = new Data(type, false, false);
        m_type = Data::Type::Undefined;
    }
    else
    {
        m_type = type;
    }
}

std::string Data::typeToString(Data::Type type)
{
    std::string stringy;
    switch (type)
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
        case Data::Type::Undefined:
            stringy = "undefined";
            break;
        case Data::Type::Void:
            stringy = "void";
            break;
        default:
            throw std::runtime_error("Data::typeToString() - Unknown type");
            break;
    }
    return stringy;
}

Data::Type Data::getNextType() const
{
    if (m_next == nullptr)
    {
        return m_type;
    }
    return m_next->getNextType();
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

void Data::setCopyOf(std::string copyOf)
{
    m_copyOf = copyOf;
}

std::string Data::stringify() const
{
    return typeToString(m_type);
}

std::string Data::stringifyWithType() const
{
    return typeToString(m_type);
}
