#include "Exp.hpp"

Exp::Exp(const unsigned lineNum, const Exp::Kind expKind) : Node::Node(lineNum, Node::Kind::Exp), m_expKind(expKind)
{

}
