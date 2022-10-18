#include "Var.hpp"

Var::Var(const unsigned lineNum, const std::string varName, Data *data) : Decl::Decl(lineNum, Decl::Kind::Var, varName, data)
{

}

std::string Var::stringify() const
{
    if (m_data == nullptr)
    {
        throw std::runtime_error("Var: stringify error: cannot stringify \'Data\' instance: instance is nullptr");
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

void Var::makeInitialized()
{
    m_isInitialized = true;
}

void Var::makeUsed()
{
    m_isUsed = true;
}

void Var::makeStatic()
{
    if (m_sibling != nullptr)
    {
        ((Var* )m_sibling)->makeStatic();
    }
    m_data->setIsStatic(true);
}
