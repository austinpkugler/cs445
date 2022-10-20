#include "Is.hpp"

bool isDecl(const Node *node)
    {
        if (node == nullptr || node->getNodeKind() != Node::Kind::Decl)
        {
            return false;
        }
        return true;
    }

bool isFunc(const Node *node)
    {
        if (!isDecl(node))
        {
            return false;
        }
        Decl *decl = (Decl *)node;
        if (decl->getDeclKind() != Decl::Kind::Func)
        {
            return false;
        }
        return true;
    }

bool isParm(const Node *node)
    {
        if (!isDecl(node))
        {
            return false;
        }
        Decl *decl = (Decl *)node;
        if (decl->getDeclKind() != Decl::Kind::Parm)
        {
            return false;
        }
        return true;
    }

bool isVar(const Node *node)
    {
        if (!isDecl(node))
        {
            return false;
        }
        Decl *decl = (Decl *)node;
        if (decl->getDeclKind() != Decl::Kind::Var)
        {
            return false;
        }
        return true;
    }

bool isExp(const Node *node)
    {
        if (node == nullptr || node->getNodeKind() != Node::Kind::Exp)
        {
            return false;
        }
        return true;
    }

bool isAsgn(const Node *node)
    {
        if (!isExp(node))
        {
            return false;
        }
        Exp *exp = (Exp *)node;
        if (exp->getExpKind() != Exp::Kind::Asgn)
        {
            return false;
        }
        return true;
    }

bool isBinary(const Node *node)
    {
        if (!isExp(node))
        {
            return false;
        }
        Exp *exp = (Exp *)node;
        if (exp->getExpKind() != Exp::Kind::Binary)
        {
            return false;
        }
        return true;
    }

bool isCall(const Node *node)
    {
        if (!isExp(node))
        {
            return false;
        }
        Exp *exp = (Exp *)node;
        if (exp->getExpKind() != Exp::Kind::Call)
        {
            return false;
        }
        return true;
    }

bool isConst(const Node *node)
    {
        if (!isExp(node))
        {
            return false;
        }
        Exp *exp = (Exp *)node;
        if (exp->getExpKind() != Exp::Kind::Const)
        {
            return false;
        }
        return true;
    }

bool isId(const Node *node)
    {
        if (!isExp(node))
        {
            return false;
        }
        Exp *exp = (Exp *)node;
        if (exp->getExpKind() != Exp::Kind::Id)
        {
            return false;
        }
        return true;
    }

bool isUnary(const Node *node)
    {
        if (!isExp(node))
        {
            return false;
        }
        Exp *exp = (Exp *)node;
        if (exp->getExpKind() != Exp::Kind::Unary)
        {
            return false;
        }
        return true;
    }

bool isUnaryAsgn(const Node *node)
    {
        if (!isExp(node))
        {
            return false;
        }
        Exp *exp = (Exp *)node;
        if (exp->getExpKind() != Exp::Kind::UnaryAsgn)
        {
            return false;
        }
        return true;
    }

bool isStmt(const Node *node)
    {
        if (node == nullptr || node->getNodeKind() != Node::Kind::Stmt)
        {
            return false;
        }
        return true;
    }

bool isBreak(const Node *node)
    {
        if (!isStmt(node))
        {
            return false;
        }
        Stmt *stmt = (Stmt *)node;
        if (stmt->getStmtKind() != Stmt::Kind::Break)
        {
            return false;
        }
        return true;
    }

bool isCompound(const Node *node)
    {
        if (!isStmt(node))
        {
            return false;
        }
        Stmt *stmt = (Stmt *)node;
        if (stmt->getStmtKind() != Stmt::Kind::Compound)
        {
            return false;
        }
        return true;
    }

bool isFor(const Node *node)
    {
        if (!isStmt(node))
        {
            return false;
        }
        Stmt *stmt = (Stmt *)node;
        if (stmt->getStmtKind() != Stmt::Kind::For)
        {
            return false;
        }
        return true;
    }

bool isIf(const Node *node)
    {
        if (!isStmt(node))
        {
            return false;
        }
        Stmt *stmt = (Stmt *)node;
        if (stmt->getStmtKind() != Stmt::Kind::If)
        {
            return false;
        }
        return true;
    }

bool isRange(const Node *node)
    {
        if (!isStmt(node))
        {
            return false;
        }
        Stmt *stmt = (Stmt *)node;
        if (stmt->getStmtKind() != Stmt::Kind::Range)
        {
            return false;
        }
        return true;
    }

bool isReturn(const Node *node)
    {
        if (!isStmt(node))
        {
            return false;
        }
        Stmt *stmt = (Stmt *)node;
        if (stmt->getStmtKind() != Stmt::Kind::Return)
        {
            return false;
        }
        return true;
    }

bool isWhile(const Node *node)
    {
        if (!isStmt(node))
        {
            return false;
        }
        Stmt *stmt = (Stmt *)node;
        if (stmt->getStmtKind() != Stmt::Kind::While)
        {
            return false;
        }
        return true;
    }
