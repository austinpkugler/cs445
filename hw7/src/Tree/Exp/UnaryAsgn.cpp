#include "UnaryAsgn.hpp"

UnaryAsgn::UnaryAsgn(const int lineNum, const UnaryAsgn::Type type) : Exp::Exp(lineNum, new Data(Data::Type::Int, false, false)), m_type(type) {}

std::string UnaryAsgn::stringify() const
{
    return "Assign: " + getSym();
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
