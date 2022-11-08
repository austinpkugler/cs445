#include "UnaryAsgn.hpp"

UnaryAsgn::UnaryAsgn(const unsigned tokenLineNum, const Type type) : Node::Node(tokenLineNum), m_type(type)
{

}

std::string UnaryAsgn::stringify() const
{
    std::string stringy = "Assign: ";
    switch(m_type)
    {
        case Type::Inc:
            stringy += "++";
            break;
        case Type::Dec:
            stringy += "--";
            break;
        default:
            throw std::runtime_error("Could not stringify UnaryAsgn::Type");
            break;
    }
    return stringy;
}
