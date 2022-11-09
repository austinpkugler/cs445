#include "SemanticEmit.hpp"

void SemanticEmit::error(const std::string type, const std::string msg)
{
    std::cout << "ERROR(" << type << "): " << msg << std::endl;
    s_errorCount++;
}

void SemanticEmit::error(const int lineNum, const std::string msg)
{
    std::cout << "ERROR(" << lineNum << "): " << msg << std::endl;
    s_errorCount++;
}

void SemanticEmit::incErrorCount(unsigned count)
{
    s_errorCount += count;
}

void SemanticEmit::warn(const std::string type, const std::string msg)
{
    std::cout << "WARNING(" << type << "): " << msg << std::endl;
    s_warnCount++;
}

void SemanticEmit::warn(const int lineNum, const std::string msg)
{
    std::cout << "WARNING(" << lineNum << "): " << msg << std::endl;
    s_warnCount++;
}

void SemanticEmit::incWarnCount(unsigned count)
{
    s_warnCount += count;
}

void SemanticEmit::count()
{
    std::cout << "Number of warnings: " << s_warnCount << std::endl;
    std::cout << "Number of errors: " << s_errorCount << std::endl;
}
