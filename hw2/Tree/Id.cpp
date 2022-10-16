#include "Id.hpp"

Id::Id(const unsigned tokenLineNum, const std::string idName, const bool isArray) : Node::Node(tokenLineNum, idName), m_isArray(isArray)
{

}

std::string Id::stringify() const
{
    return "Id: " + m_stringValue;
}
