#include "UnaryAsgn.hpp"

UnaryAsgn::UnaryAsgn(const unsigned lineNum, const UnaryAsgn::Type type) : Exp::Exp(lineNum, Exp::Kind::UnaryAsgn, new Data(Data::Type::Int, false, false)), m_type(type)
{

}

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
            throw std::runtime_error("UnaryAsgn: stringify error: cannot stringify unknown \'UnaryAsgn::Type\'");
            break;
    }
    return stringy;
}
