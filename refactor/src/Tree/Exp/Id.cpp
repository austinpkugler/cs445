#include "Id.hpp"

Id::Id(const int lineNum, const std::string isIdame, const bool isArray) : Exp::Exp(lineNum, new Data(Data::Type::Undefined, false, false)), m_name(isIdame), m_isArray(isArray) {}

std::string Id::stringify() const
{
    return "Id: " + m_name;
}

std::string Id::stringifyWithType() const
{
    std::string typeString = Data::typeToString(m_data->getNextType());
    if (typeString != "undefined")
    {
        return stringify() + " of type " + typeString;
    }
    else
    {
        return stringify() + " of undefined type";
    }
}
