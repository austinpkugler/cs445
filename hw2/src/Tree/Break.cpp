#include "Break.hpp"

Break::Break(const unsigned tokenLineNum) : Node::Node(tokenLineNum)
{

}

std::string Break::stringify() const
{
    return "Break";
}
