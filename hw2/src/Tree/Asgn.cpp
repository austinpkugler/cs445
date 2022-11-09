#include "Asgn.hpp"

Asgn::Asgn(const unsigned tokenLineNum, const Type type) : Node::Node(tokenLineNum), m_type(type)
{

}

std::string Asgn::stringify() const
{
    std::string stringy = "Assign: ";
    switch(m_type)
    {
        case Type::Asgn:
            stringy += "=";
            break;
        case Type::AddAsgn:
            stringy += "+=";
            break;
        case Type::SubAsgn:
            stringy += "-=";
            break;
        case Type::DivAsgn:
            stringy += "/=";
            break;
        case Type::MulAsgn:
            stringy += "*=";
            break;
        default:
            throw std::runtime_error("Could not stringify Asgn::Type");
            break;
    }
    return stringy;
}
