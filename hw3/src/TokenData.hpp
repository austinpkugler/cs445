// Based off CS445 - Calculator Example Program by Robert Heckendorn
#pragma once

#include <string>

struct TokenData
{
    unsigned lineNum;              // Line number of token occurrence
    std::string tokenContent;           // The string that was read
};
