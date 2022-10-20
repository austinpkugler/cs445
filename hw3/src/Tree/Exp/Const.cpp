#include "Const.hpp"

Const::Const(const unsigned lineNum, const Const::Type type, const std::string constValue) : Exp::Exp(lineNum, Exp::Kind::Const, new Data(Data::Type::None, false, false)), m_type(type)
{
    switch (m_type)
    {
        case Const::Type::Int:
            m_intValue = std::stoi(constValue);
            m_data->setType(Data::Type::Int);
            break;
        case Const::Type::Bool:
            m_boolValue = (constValue == "true");
            m_data->setType(Data::Type::Bool);
            break;
        case Const::Type::Char:
        {
            std::string chars = removeFirstAndLastChar(constValue);
            m_charValue = parseFirstChar(chars);
            if (chars.length() > 1 && chars[0] != '\\')
            {
                m_charLengthWarning = true;
            }
            m_data->setType(Data::Type::Char);
            break;
        }
        case Const::Type::String:
            m_stringValue = parseChars(removeFirstAndLastChar(constValue));
            m_data->setType(Data::Type::Char);
            m_data->setIsArray(true);
            break;
        default:
            throw std::runtime_error("Const::Const() - Unknown type");
            break;
    }
}

std::string Const::stringify() const
{
    std::string stringy = "Const ";
    switch (m_type)
    {
        case Const::Type::Int:
            stringy += std::to_string(m_intValue);
            break;
        case Const::Type::Bool:
            if (m_boolValue)
            {
                stringy += "true";
            }
            else
            {
                stringy += "false";
            }
            break;
        case Const::Type::Char:
            stringy += "'" + std::string(1, m_charValue) + "'";
            break;
        case Const::Type::String:
            stringy += "is array \"" + m_stringValue + "\"";
            break;
    }
    return stringy;
}

std::string Const::stringifyWithType() const
{
    std::string typeString = Data::typeToString(m_data->getNextType());
    if (typeString != "undefined")
    {
        return stringify() + " of type " + typeString;
    }
    else
    {
        return stringify() + " of undefined type";
    }
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
