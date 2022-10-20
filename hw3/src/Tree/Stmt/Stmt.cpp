#include "Stmt.hpp"

Stmt::Stmt(const unsigned lineNum, const Stmt::Kind stmtKind) : Node::Node(lineNum, Node::Kind::Stmt), m_stmtKind(stmtKind)
{

}
