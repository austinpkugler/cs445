#include "Unary.hpp"

Unary::Unary(const unsigned tokenLineNum, const Type type) : Node::Node(tokenLineNum), m_type(type)
{

}

std::string Unary::stringify() const
{
    std::string stringy = "Op: ";
    switch(m_type)
    {
        case Type::Chsign:
            stringy += "chsign";
            break;
        case Type::Sizeof:
            stringy += "sizeof";
            break;
        case Type::Question:
            stringy += "?";
            break;
        default:
            throw std::runtime_error("Could not stringify Unary::Type");
            break;
    }
    return stringy;
}
