#include "Exp.hpp"

Exp::Exp(const int lineNum, Data *data) : Node::Node(lineNum), m_data(data) {}

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
