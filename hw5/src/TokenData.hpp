// Based off CS445 - Calculator Example Program by Robert Heckendorn
#pragma once

#include <string>

struct TokenData
{
    int lineNum;                // Line number of token occurrence
    std::string tokenContent;   // The string that was read
};
