#include "Const.hpp"

Const::Const(const unsigned tokenLineNum, const Type type, const std::string value) : Node::Node(tokenLineNum), m_type(type)
{
    switch(m_type)
    {
        case Type::Int:
            m_intValue = std::stoi(value);
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
            m_stringValue = parseChars(removeFirstAndLastChar(value));
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

char Const::parseFirstChar(const std::string &str) const
{
    if (str.length() == 1)
    {
        return str[0];
    }

    char parsedChar;
    if (str[0] == '\\')
    {
        if (str[1] == 'n')
        {
            return '\n';
        }
        else if (str[1] == '0')
        {
            return '\0';
        }
        else
        {
            return str[1];
        }
    }
    return str[0];
}

std::string Const::removeFirstAndLastChar(const std::string &str) const
{
    return str.substr(1, str.length() - 2);
}

std::string Const::parseChars(const std::string &str) const
{
    std::string unparsedChars = str;
    std::string parsedChars;
    while (unparsedChars.length() > 0)
    {
        char currChar = unparsedChars[0];
        parsedChars += parseFirstChar(unparsedChars);
        if (currChar == '\\' && unparsedChars.length() >= 2)
        {
            unparsedChars = unparsedChars.substr(2);
        }
        else
        {
            unparsedChars = unparsedChars.substr(1);
        }
    }
    return parsedChars;
}
