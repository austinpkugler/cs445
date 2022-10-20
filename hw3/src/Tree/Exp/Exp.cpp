#include "Exp.hpp"

Exp::Exp(const unsigned lineNum, const Exp::Kind expKind, Data *data) : Node::Node(lineNum, Node::Kind::Exp), m_expKind(expKind), m_data(data)
{

}

void Exp::setData(Data *data)
{
    m_data = data;
}

std::string Exp::stringifyWithType() const
{
    return stringify() + " " + m_data->stringify();
}
