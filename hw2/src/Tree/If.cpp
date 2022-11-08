#include "If.hpp"

If::If(const unsigned tokenLineNum) : Node::Node(tokenLineNum)
{

}

std::string If::stringify() const
{
    return "If";
}
