#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class Emit
{
    public:
        static unsigned getErrorCount() { return s_errorCount; }
        static unsigned getWarnCount() { return s_warnCount; }
        static void error(const std::string type, const std::string msg);
        static void error(const int lineNum, const std::string msg, const bool isMisplaceChar=false);
        static void incErrorCount(unsigned count=1);
        static void warn(const std::string type, const std::string msg);
        static void warn(const int lineNum, const std::string msg);
        static void incWarnCount(unsigned count=1);
        static void count();

    private:
        inline static unsigned s_errorCount;
        inline static unsigned s_warnCount;
        inline static std::vector<int> s_misplacedChars;
};
