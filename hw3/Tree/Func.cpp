#include "Func.hpp"

Func::Func(const unsigned tokenLineNum, Primitive *type, const std::string funcName) : Node::Node(tokenLineNum, funcName), m_type(type)
{

}

std::string Func::stringify() const
{
    return "Func: " + m_stringValue + " returns type " + m_type->stringify();
}
