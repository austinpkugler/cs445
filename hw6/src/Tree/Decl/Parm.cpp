#include "Parm.hpp"

Parm::Parm(const int lineNum, const std::string parmName, Data *data) : Decl::Decl(lineNum, parmName, data)
{
    setHasMem(true);
    setMem("Parameter");
}

std::string Parm::stringify() const
{
    if (m_data == nullptr)
    {
        throw std::runtime_error("Parm::stringify() - Data must exist");
    }

    std::string stringWithType = "Parm: " + m_name + " of ";
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

    return stringWithType + "type " + m_data->stringify();
}
