#include "Var.hpp"

Var::Var(const int lineNum, const std::string isVarame, Data *data) : Decl::Decl(lineNum, isVarame, data), m_isInitialized(false), m_isGlobal(false) {}

std::string Var::stringify() const
{
    if (m_data == nullptr)
    {
        throw std::runtime_error("Var::stringify() - Data must exist");
    }

    std::string stringWithType = "Var: " + m_name + " of ";
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

void Var::makeStatic()
{
    if (m_sibling != nullptr)
    {
        ((Var* )m_sibling)->makeStatic();
    }
    m_data->setIsStatic(true);
}
