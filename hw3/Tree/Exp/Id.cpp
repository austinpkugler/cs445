#include "Id.hpp"

Id::Id(const unsigned lineNum, const std::string idName, const bool isArray) : Exp::Exp(lineNum, Exp::Kind::Id), m_name(idName), m_isArray(isArray)
{

}

std::string Id::stringify() const
{
    return "Id: " + m_name;
}
