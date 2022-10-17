#include "Semantics.hpp"

Semantics::Semantics(SymTable *symTable) : m_symTable(symTable)
{

}

void Semantics::analyze(Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    switch (node->getNodeKind())
    {
        case Node::NodeKind::Decl:
            analyzeDecl(node);
            break;
        case Node::NodeKind::Stmt:
            analyzeStmt(node);
            break;
        case Node::NodeKind::Exp:
            analyzeExp(node);
            break;
        case Node::NodeKind::None:
            throw std::runtime_error("Cannot analyze node of \'None\' kind");
            break;
        default:
            throw std::runtime_error("Cannot analyze node of unknown kind");
            break;
    }
}

void Semantics::analyzeDecl(Node *node)
{
    if (node == nullptr)
    {
        throw std::runtime_error("Cannot analyze nullptr node of \'Decl\' kind");
    }

    switch(node->getDeclKind())
    {
        case Node::DeclKind::Func:
        {
            Func *funcNode = (Func* )node;
            m_symTable->insert(funcNode->getStringValue(), funcNode);

            if (funcNode->getStringValue() == "main")
            {
                m_mainExists = true;
            }

            m_symTable->enter(funcNode->getStringValue());

            Var *parmNode = (Var *)(funcNode->getChildren()[0]);
            while (parmNode != nullptr)
            {
                m_symTable->insert(funcNode->getStringValue(), parmNode);
                parmNode = (Var *)(parmNode->getSibling());
            }
            break;
        }
        case Node::DeclKind::Parm:
            break;
        case Node::DeclKind::Var:
        {
            Var *varNode = (Var* )node;
            if (varNode->getIsStatic())
            {
                m_symTable->insertGlobal(varNode->getStringValue(), varNode);
            }
            else
            {
                m_symTable->insert(varNode->getStringValue(), varNode);
            }
            break;
        }
        case Node::DeclKind::DeclNone:
            throw std::runtime_error("Cannot analyze node of \'DeclNone\' \'Decl\' kind");
            break;
        default:
            throw std::runtime_error("Cannot analyze node of unknown \'Decl\' kind");
            break;
    }
}

void Semantics::analyzeStmt(Node *node) const
{
    if (node == nullptr)
    {
        throw std::runtime_error("Cannot analyze nullptr node of \'Stmt\' kind");
    }

    switch(node->getStmtKind())
    {
        case Node::StmtKind::Break:
            break;
        case Node::StmtKind::Compound:
            break;
        case Node::StmtKind::For:
            break;
        case Node::StmtKind::If:
            break;
        case Node::StmtKind::Return:
            break;
        case Node::StmtKind::While:
            break;
        case Node::StmtKind::StmtNone:
            throw std::runtime_error("Cannot analyze node of \'StmtNone\' \'Stmt\' kind");
            break;
        default:
            throw std::runtime_error("Cannot analyze node of unknown \'Stmt\' kind");
            break;
    }
}

void Semantics::analyzeExp(Node *node) const
{
    if (node == nullptr)
    {
        throw std::runtime_error("Cannot analyze nullptr node of \'Exp\' kind");
    }

    switch(node->getExpKind())
    {
        case Node::ExpKind::Asgn:
            break;
        case Node::ExpKind::Binary:
            break;
        case Node::ExpKind::Call:
            break;
        case Node::ExpKind::Const:
            break;
        case Node::ExpKind::Id:
            break;
        case Node::ExpKind::Range:
            break;
        case Node::ExpKind::Unary:
            break;
        case Node::ExpKind::UnaryAsgn:
            break;
        case Node::ExpKind::ExpNone:
            throw std::runtime_error("Cannot analyze node of \'ExpNone\' \'Exp\' kind");
            break;
        default:
            throw std::runtime_error("Cannot analyze node of unknown \'Exp\' kind");
            break;
    }
}
