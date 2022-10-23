#include "Func.hpp"

Func::Func(const int lineNum, const std::string funcName, Data *data) : Decl::Decl(lineNum, funcName, data) {}

std::string Func::stringify() const
{
    return "Func: " + m_name + " returns type " + m_data->stringify();
}
