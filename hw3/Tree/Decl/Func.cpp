#include "Func.hpp"

Func::Func(const unsigned lineNum, const std::string isFuncame, Data *data) : Decl::Decl(lineNum, Decl::Kind::Func, isFuncame, data)
{

}

std::string Func::stringify() const
{
    return "Func: " + m_name + " returns type " + m_data->stringify();
}
