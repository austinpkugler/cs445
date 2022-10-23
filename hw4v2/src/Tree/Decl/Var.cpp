#include "Var.hpp"

Var::Var(const int lineNum, const std::string isVarame, Data *data) : Decl::Decl(lineNum, isVarame, data), m_isInitialized(false) {}

std::string Var::stringify() const
{
    if (m_data == nullptr)
    {
        throw std::runtime_error("Var::stringify() - Data must exist");
    }

    if (m_data->getIsArray() && m_data->getIsStatic())
    {
        return "Var: " + m_name + " is array of type " + m_data->stringify();
    }
    else if (m_data->getIsArray())
    {
        return "Var: " + m_name + " is array of type " + m_data->stringify();
    }
    else if (m_data->getIsStatic())
    {
        return "Var: " + m_name + " of type " + m_data->stringify();
    }
    return "Var: " + m_name + " of type " + m_data->stringify();
}

void Var::makeStatic()
{
    if (m_sibling != nullptr)
    {
        ((Var* )m_sibling)->makeStatic();
    }
    m_data->setIsStatic(true);
}
