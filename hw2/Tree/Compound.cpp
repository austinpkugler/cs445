#include "Compound.hpp"

Compound::Compound(const unsigned tokenLineNum) : Node::Node(tokenLineNum)
{

}

std::string Compound::stringify() const
{
    return "Compound: ";
}
