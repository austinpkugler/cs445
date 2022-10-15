#include "Range.hpp"

Range::Range(const unsigned tokenLineNum) : Node::Node(tokenLineNum)
{

}

std::string Range::stringify() const
{
    return "Range";
}
