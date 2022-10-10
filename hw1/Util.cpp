#include "Util.hpp"

#include <algorithm>

char Util::parseFirstChar(const std::string &str)
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

std::string Util::toUpper(const std::string &str)
{
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    return upperStr;
}

std::string Util::removeFirstAndLastChar(const std::string &str)
{
    return str.substr(1, str.length() - 2);
}

std::string Util::parseChars(const std::string &str)
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

// int Util::countChars(const std::string &str)
// {
//     std::string unparsedChars = str;
//     int charCount;
//     while (unparsedChars.length() > 0)
//     {
//         char currChar = unparsedChars[0];
//         charCount += 1;
//         if (currChar == '\\' && unparsedChars.length() >= 2)
//         {
//             unparsedChars = unparsedChars.substr(2);
//         }
//         else
//         {
//             unparsedChars = unparsedChars.substr(1);
//         }
//     }
//     return charCount;
// }
