#include "Const.hpp"

Const::Const(unsigned tokenLineNum, const Type type, const std::string value) : Node::Node(tokenLineNum), m_type(type)
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

void Const::printNode() const
{
    std::cout << "Const ";
    switch(m_type)
    {
        case Type::Int:
            std::cout << m_intValue;
            break;
        case Type::Bool:
            if (m_boolValue)
            {
                std::cout << "true";
            }
            else
            {
                std::cout << "false";
            }
            break;
        case Type::Char:
            std::cout << "'" << m_charValue << "'";
            break;
        case Type::String:
            std::cout << "\"" << m_stringValue << "\"";
            break;
    }
}
