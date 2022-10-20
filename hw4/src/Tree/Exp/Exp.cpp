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
    std::string typeString = m_data->stringifyWithType();
    if (typeString != "undefined")
    {
        return stringify() + " of type " + typeString;
    }
    else
    {
        return stringify() + " of undefined type";
    }
}
