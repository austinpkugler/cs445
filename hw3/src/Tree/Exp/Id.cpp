#include "Id.hpp"

Id::Id(const unsigned lineNum, const std::string isIdame, const bool isArray) : Exp::Exp(lineNum, Exp::Kind::Id, new Data(Data::Type::None, false, false)), m_name(isIdame), m_isArray(isArray)
{

}

std::string Id::stringify() const
{
    return "Id: " + m_name;
}
