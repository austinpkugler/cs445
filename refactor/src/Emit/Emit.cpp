#include "Emit.hpp"

void Emit::error(const std::string type, const std::string msg)
{
    std::cout << "ERROR(" << type << "): " << msg << std::endl;
    s_errorCount++;
}

void Emit::error(const int lineNum, const std::string msg)
{
    std::cout << "ERROR(" << lineNum << "): " << msg << std::endl;
    s_errorCount++;
}

void Emit::warn(const std::string type, const std::string msg)
{
    std::cout << "WARNING(" << type << "): " << msg << std::endl;
    s_warnCount++;
}

void Emit::warn(const int lineNum, const std::string msg)
{
    std::cout << "WARNING(" << lineNum << "): " << msg << std::endl;
    s_warnCount++;
}

void Emit::count()
{
    std::cout << "Number of warnings: " << s_warnCount << std::endl;
    std::cout << "Number of errors: " << s_errorCount << std::endl;
}
