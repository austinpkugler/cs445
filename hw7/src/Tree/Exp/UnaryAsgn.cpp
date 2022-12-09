#include "UnaryAsgn.hpp"

UnaryAsgn::UnaryAsgn(const int lineNum, const UnaryAsgn::Type type) : Exp::Exp(lineNum, new Data(Data::Type::Int, false, false)), m_type(type) {}

std::string UnaryAsgn::stringify() const
{
    return "Assign: " + getSym();
}

std::string UnaryAsgn::getTypeString() const
{
    std::string stringy;
    switch (m_type)
    {
        case Type::Inc:
            stringy = "increment";
            break;
        case Type::Dec:
            stringy = "decrement";
            break;
        default:
            throw std::runtime_error("UnaryAsgn:getTypeString() - Unknown type");
            break;
    }
    return stringy;
}

int UnaryAsgn::getTypeValue() const
{
    int value;
    switch (m_type)
    {
        case Type::Inc:
            value = 1;
            break;
        case Type::Dec:
            value = -1;
            break;
        default:
            throw std::runtime_error("UnaryAsgn:getTypeValue() - Unknown type");
            break;
    }
    return value;
}

std::string UnaryAsgn::getSym() const
{
    std::string stringy;
    switch (m_type)
    {
        case Type::Inc:
            stringy = "++";
            break;
        case Type::Dec:
            stringy = "--";
            break;
        default:
            throw std::runtime_error("UnaryAsgn:getSym() - Unknown type");
            break;
    }
    return stringy;
}
