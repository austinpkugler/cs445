#include "Parm.hpp"

Parm::Parm(const int lineNum, const std::string parmName, Data *data) : Decl::Decl(lineNum, parmName, data) {}

std::string Parm::stringify() const
{
    if (m_data == nullptr)
    {
        throw std::runtime_error("Parm::stringify() - Data must exist");
    }

    if (m_data->getIsArray())
    {
        return "Parm: " + m_name + " is array of type " + m_data->stringify();
    }
    return "Parm: " + m_name + " of type " + m_data->stringify();
}
