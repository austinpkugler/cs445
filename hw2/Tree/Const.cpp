#include "Const.hpp"

Const::Const(const unsigned tokenLineNum, const Type type, const std::string value) : Node::Node(tokenLineNum), m_type(type)
{
    switch(m_type)
    {
        case Type::Int:
            m_intValue = std::atoi(value.c_str());
            break;
        case Type::Bool:
            m_boolValue = (value == "true");
            break;
        case Type::Char:
        {
            std::string chars = removeFirstAndLastChar(value);
            m_charValue = parseFirstChar(chars);
            if (chars.length() > 1 && chars[0] != '\\')
            {
                m_charLengthWarning = true;
            }
            break;
        }
        case Type::String:
            m_stringValue = removeFirstAndLastChar(value);
            break;
    }
}

std::string Const::stringify() const
{
    std::string stringy = "Const ";
    switch(m_type)
    {
        case Type::Int:
            stringy += std::to_string(m_intValue);
            break;
        case Type::Bool:
            if (m_boolValue)
            {
                stringy += "true";
            }
            else
            {
                stringy += "false";
            }
            break;
        case Type::Char:
            stringy += "'" + std::string(1, m_charValue) + "'";
            break;
        case Type::String:
            stringy += "\"" + m_stringValue + "\"";
            break;
    }
    return stringy;
}
