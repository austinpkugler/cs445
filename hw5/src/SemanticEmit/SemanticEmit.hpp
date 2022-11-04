#pragma once

#include <iostream>
#include <string>

class SemanticEmit
{
    public:
        static unsigned getErrorCount() { return s_errorCount; }
        static unsigned getWarnCount() { return s_warnCount; }
        static void error(const std::string type, const std::string msg);
        static void error(const int lineNum, const std::string msg);
        static void incErrorCount(unsigned count=1);
        static void warn(const std::string type, const std::string msg);
        static void warn(const int lineNum, const std::string msg);
        static void incWarnCount(unsigned count=1);
        static void count();

    private:
        inline static unsigned s_errorCount;
        inline static unsigned s_warnCount;
};
