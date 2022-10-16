#include "Id.hpp"

Id::Id(const unsigned tokenLineNum, const std::string value, const bool isArray) : Node::Node(tokenLineNum, value), m_isArray(isArray)
{

}

std::string Id::stringify() const
{
    return "Id: ";
}
