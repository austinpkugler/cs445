#include "Binary.hpp"

Binary::Binary(const unsigned tokenLineNum, const Type type) : Node::Node(tokenLineNum), m_type(type)
{

}

std::string Binary::stringify() const
{
    std::string stringy = "Op: ";
    switch(m_type)
    {
        case Type::Mul:
            stringy += "*";
            break;
        case Type::Div:
            stringy += "/";
            break;
        case Type::Mod:
            stringy += "%";
            break;
        case Type::Add:
            stringy += "+";
            break;
        case Type::Sub:
            stringy += "-";
            break;
        case Type::Index:
            stringy += "[";
            break;
        case Type::And:
            stringy += "and";
            break;
        case Type::Or:
            stringy += "or";
            break;
        case Type::LT:
            stringy += "<";
            break;
        case Type::LEQ:
            stringy += "<=";
            break;
        case Type::GT:
            stringy += ">";
            break;
        case Type::GEQ:
            stringy += ">=";
            break;
        case Type::EQ:
            stringy += "==";
            break;
        case Type::NEQ:
            stringy += "!=";
            break;
        default:
            throw std::runtime_error("Could not stringify Binary::Type");
            break;
    }
    return stringy;
}
