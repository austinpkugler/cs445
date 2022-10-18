#include "Decl.hpp"

Decl::Decl(const unsigned lineNum, const Decl::Kind declKind, const std::string name, Data *data) : Node::Node(lineNum, Node::Kind::Decl), m_declKind(declKind), m_name(name), m_data(data)
{

}

void Decl::setType(const Data::Type type)
{
    m_data->setType(type);
    if (m_sibling != nullptr)
    {
        Decl *node = (Decl *)m_sibling;
        node->setType(type);
    }
}
