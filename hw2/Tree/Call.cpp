#include "Call.hpp"

Call::Call(const unsigned tokenLineNum, const std::string value) : Node::Node(tokenLineNum, value)
{

}

std::string Call::stringify() const
{
    return "Call: " + m_stringValue;
}
