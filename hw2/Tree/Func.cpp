#include "Func.hpp"

Func::Func(const unsigned tokenLineNum, const Primitive::Type type, const std::string value) : Node::Node(tokenLineNum, value), m_type(type)
{

}

void Func::printNode() const
{
    std::cout << stringify();
    m_type.printNode();
}

std::string Func::stringify() const
{
    return "Func" + m_stringValue + " returns ";
}
