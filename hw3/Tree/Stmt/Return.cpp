#include "Return.hpp"

Return::Return(const unsigned tokenLineNum) : Node::Node(tokenLineNum)
{

}

std::string Return::stringify() const
{
    return "Return";
}
