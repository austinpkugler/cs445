#include "UnaryAsgn.hpp"

UnaryAsgn::UnaryAsgn(const unsigned lineNum, const UnaryAsgn::Type type) : Exp::Exp(lineNum, Exp::Kind::UnaryAsgn), m_type(type)
{

}

std::string UnaryAsgn::stringify() const
{
    std::string stringy = "Assign: ";
    switch (m_type)
    {
        case Type::Inc:
            stringy += "++";
            break;
        case Type::Dec:
            stringy += "--";
            break;
        default:
            throw std::runtime_error("UnaryAsgn: stringify error: cannot stringify unknown \'UnaryAsgn::Type\'");
            break;
    }
    return stringy;
}
