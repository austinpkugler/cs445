#include "Is.hpp"

bool isDecl(const Node *node)
{
    return (node != nullptr && node->getNodeKind() == Node::Kind::Decl);
}

bool isFunc(const Node *node)
{
    if (!isDecl(node)) return false;
    return (((Decl *)node)->getDeclKind() == Decl::Kind::Func);
}

bool isParm(const Node *node)
{
    if (!isDecl(node)) return false;
    return (((Decl *)node)->getDeclKind() == Decl::Kind::Parm);
}

bool isVar(const Node *node)
{
    if (!isDecl(node)) return false;
    return (((Decl *)node)->getDeclKind() == Decl::Kind::Var);
}

bool isExp(const Node *node)
{
    return (node != nullptr && node->getNodeKind() == Node::Kind::Exp);
}

bool isAsgn(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getExpKind() == Exp::Kind::Asgn);
}

bool isBinary(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getExpKind() == Exp::Kind::Binary);
}

bool isCall(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getExpKind() == Exp::Kind::Call);
}

bool isConst(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getExpKind() == Exp::Kind::Const);
}

bool isId(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getExpKind() == Exp::Kind::Id);
}

bool isUnary(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getExpKind() == Exp::Kind::Unary);
}

bool isUnaryAsgn(const Node *node)
{
    if (!isExp(node)) return false;
    return (((Exp *)node)->getExpKind() == Exp::Kind::UnaryAsgn);
}

bool isStmt(const Node *node)
{
    return (node != nullptr && node->getNodeKind() == Node::Kind::Stmt);
}

bool isBreak(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getStmtKind() == Stmt::Kind::Break);
}

bool isCompound(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getStmtKind() == Stmt::Kind::Compound);
}

bool isFor(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getStmtKind() == Stmt::Kind::For);
}

bool isIf(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getStmtKind() == Stmt::Kind::If);
}

bool isRange(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getStmtKind() == Stmt::Kind::Range);
}

bool isReturn(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getStmtKind() == Stmt::Kind::Return);
}

bool isWhile(const Node *node)
{
    if (!isStmt(node)) return false;
    return (((Stmt *)node)->getStmtKind() == Stmt::Kind::While);
}
