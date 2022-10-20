#include "Binary.hpp"

Binary::Binary(const unsigned lineNum, const Binary::Type type) : Exp::Exp(lineNum, Exp::Kind::Binary, new Data(Data::Type::None, false, false)), m_type(type)
{
    switch (m_type)
    {
        case Binary::Type::Mul:
        case Binary::Type::Div:
        case Binary::Type::Mod:
        case Binary::Type::Add:
        case Binary::Type::Sub:
           m_data->setType(Data::Type::Int);
           break;
        case Binary::Type::Index:
            break;
        case Binary::Type::And:
        case Binary::Type::Or:
        case Binary::Type::LT:
        case Binary::Type::LEQ:
        case Binary::Type::GT:
        case Binary::Type::GEQ:
        case Binary::Type::EQ:
        case Binary::Type::NEQ:
            m_data->setType(Data::Type::Bool);
            break;
        default:
            throw std::runtime_error("Binary: constructor error: cannot type unknown \'Binary::Type\'");
            break;
    }
}

std::string Binary::stringify() const
{
    return "Op: " + getSym();
}

std::string Binary::getSym() const
{
    std::string stringy;
    switch (m_type)
    {
        case Binary::Type::Mul:
            stringy = "*";
            break;
        case Binary::Type::Div:
            stringy = "/";
            break;
        case Binary::Type::Mod:
            stringy = "%";
            break;
        case Binary::Type::Add:
            stringy = "+";
            break;
        case Binary::Type::Sub:
            stringy = "-";
            break;
        case Binary::Type::Index:
            stringy = "[";
            break;
        case Binary::Type::And:
            stringy = "and";
            break;
        case Binary::Type::Or:
            stringy = "or";
            break;
        case Binary::Type::LT:
            stringy = "<";
            break;
        case Binary::Type::LEQ:
            stringy = "<=";
            break;
        case Binary::Type::GT:
            stringy = ">";
            break;
        case Binary::Type::GEQ:
            stringy = ">=";
            break;
        case Binary::Type::EQ:
            stringy = "==";
            break;
        case Binary::Type::NEQ:
            stringy = "!=";
            break;
        default:
            throw std::runtime_error("Binary: stringify error: cannot stringify unknown \'Binary::Type\'");
            break;
    }
    return stringy;
}
