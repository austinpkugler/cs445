#include "Decl.hpp"

Decl::Decl(const int lineNum, const std::string name, Data *data) : Node::Node(lineNum), m_name(name), m_data(data), m_showErrors(true), m_isUsed(false) {}

void Decl::setType(const Data::Type type)
{
    m_data->setType(type);
    if (m_sibling != nullptr)
    {
        Decl *node = (Decl *)m_sibling;
        node->setType(type);
    }
}
