#pragma once

#include <string>

class Util
{
    public:
        static char parseFirstChar(const std::string &str);
        static std::string toUpper(const std::string &str);
        static std::string removeFirstAndLastChar(const std::string &str);
        static std::string parseChars(const std::string &str);
        // static int countChars(const std::string &str);
};
