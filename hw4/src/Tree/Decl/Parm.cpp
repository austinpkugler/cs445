#include "Parm.hpp"

Parm::Parm(const unsigned lineNum, const std::string isParmame, Data *data) : Decl::Decl(lineNum, Decl::Kind::Parm, isParmame, data)
{

}

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

void Parm::makeUsed()
{
    m_isUsed = true;
}
