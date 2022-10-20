#pragma once

#include <string>
#include <iostream>

namespace Emit
{
    class Error
    {
        public:
            static unsigned getErrorCount() { return s_errorCount; }
            static void count();
            static void generic(const unsigned lineNum, const std::string msg);
            static void linker(const std::string msg);
            static void undefinedMain();

        private:
            // Count of the errors emitted
            inline static unsigned s_errorCount;
    };

    class Warn
    {
        public:
            static unsigned getErrorCount() { return s_warnCount; }
            static void count();
            static void generic(const unsigned lineNum, const std::string msg);

        private:
            // Count of the warns emitted
            inline static unsigned s_warnCount;
    };
};
