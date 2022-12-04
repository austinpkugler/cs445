#include "Is.hpp"

bool isDecl(const Node *node)
{
    if (node == nullptr)
    {
        return false;
    }
    switch (node->getNodeKind())
    {
        case Node::Kind::Func:
        case Node::Kind::Parm:
        case Node::Kind::Var:
            return true;
        default:
            return false;
    }
}

bool isFunc(const Node *node)
{
    if (!isDecl(node)) return false;
    return (((Decl *)node)->getNodeKind() == Node::Kind::Func);
}

bool isParm(const Node *node)
{
    if (!isDecl(node)) return false;
    return (((Decl *)node)->getNodeKind() == Node::Kind::Parm);
}

bool isVar(const Node *node)
{
    if (!isDecl(node)) return false;
    return (((Decl *)node)->getNodeKind() == Node::Kind::Var);
}

bool isExp(const Node *node)
{
    if (node == nullptr)
    {
        return false;
    }
    switch (node->getNodeKind())
    {
        case Node::Kind::Asgn:
        case Node::Kind::Binary:
        case Node::Kind::Call:
        case Node::Kind::Const:
        case Node::Kind::Id:
        case Node::Kind::Unary:
        case Node::Kind::UnaryAsgn:
            return true;
        default:
            return false;
    }
}

bool isAsgn(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getNodeKind() == Node::Kind::Asgn);
}

bool isBinary(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getNodeKind() == Node::Kind::Binary);
}

bool isCall(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getNodeKind() == Node::Kind::Call);
}

bool isConst(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getNodeKind() == Node::Kind::Const);
}

bool isId(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getNodeKind() == Node::Kind::Id);
}

bool isUnary(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getNodeKind() == Node::Kind::Unary);
}

bool isUnaryAsgn(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getNodeKind() == Node::Kind::UnaryAsgn);
}

bool isStmt(const Node *node)
{
    if (node == nullptr)
    {
        return false;
    }
    switch (node->getNodeKind())
    {
        case Node::Kind::Break:
        case Node::Kind::Compound:
        case Node::Kind::For:
        case Node::Kind::If:
        case Node::Kind::Range:
        case Node::Kind::Return:
        case Node::Kind::While:
            return true;
        default:
            return false;
    }
}

bool isBreak(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getNodeKind() == Node::Kind::Break);
}

bool isCompound(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getNodeKind() == Node::Kind::Compound);
}

bool isFor(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getNodeKind() == Node::Kind::For);
}

bool isIf(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getNodeKind() == Node::Kind::If);
}

bool isRange(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getNodeKind() == Node::Kind::Range);
}

bool isReturn(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getNodeKind() == Node::Kind::Return);
}

bool isWhile(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getNodeKind() == Node::Kind::While);
}
