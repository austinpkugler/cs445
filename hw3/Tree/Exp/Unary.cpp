#include "Unary.hpp"

Unary::Unary(const unsigned lineNum, const Unary::Type type) : Exp::Exp(lineNum, Exp::Kind::Unary, new Data(Data::Type::None, false, false)), m_type(type)
{
    switch (m_type)
    {
        case Unary::Type::Chsign:
        case Unary::Type::Sizeof:
        case Unary::Type::Question:
            m_data->setType(Data::Type::Int);
            break;
        case Unary::Type::Not:
            m_data->setType(Data::Type::Bool);
            break;
        default:
            throw std::runtime_error("Unary: constructor error: cannot type unknown \'Unary::Type\'");
            break;
    }
}

std::string Unary::stringify() const
{
    std::string stringy = "Op: ";
    switch (m_type)
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
            throw std::runtime_error("Unary: stringify error: cannot stringify unknown \'Unary::Type\'");
            break;
    }
    return stringy;
}
