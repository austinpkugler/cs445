#include "Binary.hpp"

Binary::Binary(const int lineNum, const Binary::Type type) : Exp::Exp(lineNum, new Data(Data::Type::Undefined, false, false)), m_type(type)
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
            throw std::runtime_error("Binary::Binary() - Unknown type");
            break;
    }
}

std::string Binary::stringify() const
{
    return "Op: " + getSym();
}

std::string Binary::getTypeString() const
{
    std::string stringy;
    switch (m_type)
    {
        case Binary::Type::Mul:
            stringy = "MUL";
            break;
        case Binary::Type::Div:
            stringy = "DIV";
            break;
        case Binary::Type::Mod:
            stringy = "MOD";
            break;
        case Binary::Type::Add:
            stringy = "ADD";
            break;
        case Binary::Type::Sub:
            stringy = "SUB";
            break;
        case Binary::Type::Index:
            stringy = "INDEX";
            break;
        case Binary::Type::And:
            stringy = "AND";
            break;
        case Binary::Type::Or:
            stringy = "OR";
            break;
        case Binary::Type::LT:
            stringy = "TLT";
            break;
        case Binary::Type::LEQ:
            stringy = "TLE";
            break;
        case Binary::Type::GT:
            stringy = "TGT";
            break;
        case Binary::Type::GEQ:
            stringy = "TGE";
            break;
        case Binary::Type::EQ:
            stringy = "TEQ";
            break;
        case Binary::Type::NEQ:
            stringy = "TNE";
            break;
        default:
            throw std::runtime_error("Binary:getTypeString() - Unknown type");
            break;
    }
    return stringy;
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
            throw std::runtime_error("Binary:getSym() - Unknown type");
            break;
    }
    return stringy;
}

bool Binary::getIsComparison() const
{
    bool isComparison = false;
    switch (m_type)
    {
        case Binary::Type::Mul:
        case Binary::Type::Div:
        case Binary::Type::Mod:
        case Binary::Type::Add:
        case Binary::Type::Sub:
        case Binary::Type::Index:
        case Binary::Type::And:
        case Binary::Type::Or:
            break;
        case Binary::Type::LT:
        case Binary::Type::LEQ:
        case Binary::Type::GT:
        case Binary::Type::GEQ:
        case Binary::Type::EQ:
        case Binary::Type::NEQ:
            isComparison = true;
            break;
        default:
            throw std::runtime_error("Binary:getIsComparison() - Unknown type");
            break;
    }
    return isComparison;
}
