#include "Emit.hpp"

/**
 * Error
 */
void Emit::Error::count()
{
    std::cout << "Number of errors: " << s_errorCount << std::endl;
}

void Emit::Error::generic(const unsigned lineNum, const std::string msg)
{
    std::cout << "ERROR(" << lineNum << "): " << msg << std::endl;
    s_errorCount++;
}

void Emit::Error::linker(const std::string msg)
{
    std::cout << "ERROR(LINKER): " << msg << std::endl;
    s_errorCount++;
}

void Emit::Error::arglist(const std::string msg)
{
    std::cout << "ERROR(ARGLIST): " << msg << std::endl;
    s_errorCount++;
}

void Emit::Error::undefinedMain()
{
    linker("A function named 'main()' must be defined.");
}

/**
 * Warn
 */
void Emit::Warn::count()
{
    std::cout << "Number of warnings: " << s_warnCount << std::endl;
}

void Emit::Warn::generic(const unsigned lineNum, const std::string msg)
{
    std::cout << "WARNING(" << lineNum << "): " << msg << std::endl;
    s_warnCount++;
}
