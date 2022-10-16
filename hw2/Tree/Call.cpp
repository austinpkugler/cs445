#include "Call.hpp"

Call::Call(const unsigned tokenLineNum, const std::string funcName) : Node::Node(tokenLineNum, funcName)
{

}

std::string Call::stringify() const
{
    return "Call: " + m_stringValue;
}
