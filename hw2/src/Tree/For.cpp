#include "For.hpp"

For::For(const unsigned tokenLineNum) : Node::Node(tokenLineNum)
{

}

std::string For::stringify() const
{
    return "For";
}
