#include "Unary.hpp"

Unary::Unary(const unsigned lineNum, const Unary::Type type) : Exp::Exp(lineNum, Exp::Kind::Unary), m_type(type)
{

}

std::string Unary::stringify() const
{
    std::string stringy = "Op: ";
    switch(m_type)
    {
        case Unary::Type::Chsign:
            stringy += "chsign";
            break;
        case Unary::Type::Sizeof:
            stringy += "sizeof";
            break;
        case Unary::Type::Question:
            stringy += "?";
            break;
        case Unary::Type::Not:
            stringy += "not";
            break;
        default:
            throw std::runtime_error("Could not stringify Unary::Type");
            break;
    }
    return stringy;
}
