#include "Asgn.hpp"

Asgn::Asgn(const unsigned lineNum, const Asgn::Type type) : Exp::Exp(lineNum, Exp::Kind::Asgn), m_type(type)
{

}

std::string Asgn::stringify() const
{
    std::string stringy = "Assign: ";
    switch (m_type)
    {
        case Asgn::Type::Asgn:
            stringy += "=";
            break;
        case Asgn::Type::AddAsgn:
            stringy += "+=";
            break;
        case Asgn::Type::SubAsgn:
            stringy += "-=";
            break;
        case Asgn::Type::DivAsgn:
            stringy += "/=";
            break;
        case Asgn::Type::MulAsgn:
            stringy += "*=";
            break;
        default:
            throw std::runtime_error("Could not stringify Asgn::Type");
            break;
    }
    return stringy;
}
