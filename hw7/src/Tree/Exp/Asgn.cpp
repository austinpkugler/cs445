#include "Asgn.hpp"

Asgn::Asgn(const int lineNum, const Asgn::Type type) : Exp::Exp(lineNum, new Data(Data::Type::Undefined, false, false)), m_type(type)
{
    switch (m_type)
    {
        case Asgn::Type::Asgn:
            break;
        case Asgn::Type::AddAsgn:
        case Asgn::Type::SubAsgn:
        case Asgn::Type::DivAsgn:
        case Asgn::Type::MulAsgn:
            m_data->setType(Data::Type::Int);
            break;
        default:
            throw std::runtime_error("Asgn::Asgn() - Unknown type");
            break;
    }
}

std::string Asgn::stringify() const
{
    if (m_data->getIsArray())
    {
        return "Assign: " + getSym() + " of array";
    }
    else
    {
        return "Assign: " + getSym();
    }
}

std::string Asgn::getTypeString() const
{
    std::string stringy;
    switch (m_type)
    {
        case Asgn::Type::Asgn:
            stringy = "ASGN";
            break;
        case Asgn::Type::AddAsgn:
            stringy = "ADD";
            break;
        case Asgn::Type::SubAsgn:
            stringy = "SUB";
            break;
        case Asgn::Type::DivAsgn:
            stringy = "DIV";
            break;
        case Asgn::Type::MulAsgn:
            stringy = "MUL";
            break;
        default:
            throw std::runtime_error("Asgn::getSym() - Unknown type");
            break;
    }
    return stringy;
}

std::string Asgn::getSym() const
{
    std::string stringy;
    switch (m_type)
    {
        case Asgn::Type::Asgn:
            stringy = "=";
            break;
        case Asgn::Type::AddAsgn:
            stringy = "+=";
            break;
        case Asgn::Type::SubAsgn:
            stringy = "-=";
            break;
        case Asgn::Type::DivAsgn:
            stringy = "/=";
            break;
        case Asgn::Type::MulAsgn:
            stringy = "*=";
            break;
        default:
            throw std::runtime_error("Asgn::getSym() - Unknown type");
            break;
    }
    return stringy;
}
