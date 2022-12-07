#include "Emit.hpp"

void Emit::error(const std::string type, const std::string msg)
{
    if (s_verbose)
    {
        std::cout << "ERROR(" << type << "): " << msg << std::endl;
    }
    s_errorCount++;
}

void Emit::error(const int lineNum, const std::string msg, const bool isMisplaceChar)
{
    if (isMisplaceChar && !std::count(s_misplacedChars.begin(), s_misplacedChars.end(), lineNum))
    {
        s_misplacedChars.push_back(lineNum);
    }

    if (s_verbose)
    {
        std::cout << "ERROR(" << lineNum << "): " << msg << std::endl;
    }
    s_errorCount++;
}

void Emit::incErrorCount(unsigned count)
{
    s_errorCount += count;
}

void Emit::warn(const std::string type, const std::string msg)
{
    if (s_verbose)
    {
        std::cout << "WARNING(" << type << "): " << msg << std::endl;
    }
    s_warnCount++;
}

void Emit::warn(const int lineNum, const std::string msg)
{
    if (!std::count(s_misplacedChars.begin(), s_misplacedChars.end(), lineNum))
    {
        if (s_verbose)
        {
            std::cout << "WARNING(" << lineNum << "): " << msg << std::endl;
        }
        s_warnCount++;
    }
}

void Emit::incWarnCount(unsigned count)
{
    s_warnCount += count;
}

void Emit::count()
{
    if (s_verbose)
    {
        std::cout << "Number of warnings: " << s_warnCount << std::endl;
        std::cout << "Number of errors: " << s_errorCount << std::endl;
    }
}
