#include "Id.hpp"

Id::Id(const unsigned tokenLineNum, const std::string value, const bool isArray, const int arraySize) : Node::Node(tokenLineNum, value), m_isArray(isArray), m_arraySize(arraySize)
{

}

std::string Id::stringify() const
{
    return "Id: ";
}
