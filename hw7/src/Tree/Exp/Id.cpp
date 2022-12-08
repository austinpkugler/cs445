#include "Id.hpp"

Id::Id(const int lineNum, const std::string isIdame) : Exp::Exp(lineNum, new Data(Data::Type::Undefined, false, false)), m_name(isIdame), m_isGlobal(false)
{
    setMemExists(true);
}

std::string Id::stringify() const
{
    return "Id: " + m_name;
}

std::string Id::stringifyWithType() const
{
    if (m_data == nullptr)
    {
        throw std::runtime_error("Id::stringifyWithType() - Data must exist");
    }

    std::string stringWithType = "Id: " + m_name + " of ";
    if (m_data->getIsStatic() && m_data->getIsArray())
    {
        stringWithType += "static array of ";
    }
    else if (m_data->getIsStatic())
    {
        stringWithType += "static ";
    }
    else if (m_data->getIsArray())
    {
        stringWithType += "array of ";
    }

    std::string typeString = Data::typeToString(m_data->getNextType());
    if (typeString != "undefined")
    {
        return stringWithType + "type " + typeString;
    }
    else
    {
        return stringWithType + "of undefined type ";
    }
}
