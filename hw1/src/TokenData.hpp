// Based off CS445 - Calculator Example Program by Robert Heckendorn
#pragma once

#include <string>

struct TokenData {
    unsigned tokenLineNum;              // Line number of token occurrence
    std::string tokenContent;           // The string that was read
    bool charLengthWarning = false;     // True if length of char > 1

    // Token datatypes
    int numValue;
    char charValue;
    std::string stringValue;
};
