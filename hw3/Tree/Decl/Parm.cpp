#include "Parm.hpp"

Parm::Parm(const unsigned lineNum, const std::string parmName, Data *data) : Decl::Decl(lineNum, Decl::Kind::Parm, parmName, data)
{

}

std::string Parm::stringify() const
{
    if (m_data->getIsArray())
    {
        return "Parm: " + m_name + " is array of type " + m_data->stringify();
    }
    return "Parm: " + m_name + " of type " + m_data->stringify();
}
