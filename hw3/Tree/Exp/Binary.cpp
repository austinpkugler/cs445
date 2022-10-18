#include "Binary.hpp"

Binary::Binary(const unsigned lineNum, const Binary::Type type) : Exp::Exp(lineNum, Exp::Kind::Binary), m_type(type)
{

}

std::string Binary::stringify() const
{
    std::string stringy = "Op: ";
    switch (m_type)
    {
        case Binary::Type::Mul:
            stringy += "*";
            break;
        case Binary::Type::Div:
            stringy += "/";
            break;
        case Binary::Type::Mod:
            stringy += "%";
            break;
        case Binary::Type::Add:
            stringy += "+";
            break;
        case Binary::Type::Sub:
            stringy += "-";
            break;
        case Binary::Type::Index:
            stringy += "[";
            break;
        case Binary::Type::And:
            stringy += "and";
            break;
        case Binary::Type::Or:
            stringy += "or";
            break;
        case Binary::Type::LT:
            stringy += "<";
            break;
        case Binary::Type::LEQ:
            stringy += "<=";
            break;
        case Binary::Type::GT:
            stringy += ">";
            break;
        case Binary::Type::GEQ:
            stringy += ">=";
            break;
        case Binary::Type::EQ:
            stringy += "==";
            break;
        case Binary::Type::NEQ:
            stringy += "!=";
            break;
        default:
            throw std::runtime_error("Could not stringify Binary::Binary::Type");
            break;
    }
    return stringy;
}
