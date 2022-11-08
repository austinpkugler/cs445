#include "While.hpp"

While::While(const unsigned tokenLineNum) : Node::Node(tokenLineNum)
{

}

std::string While::stringify() const
{
    return "While";
}
