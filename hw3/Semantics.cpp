#include "Semantics.hpp"

Semantics::Semantics(SymTable *symTable) : m_symTable(symTable)
{

}

void Semantics::analyze(Node *node)
{
    analyzeTree(node);
    if (!m_validMainExists)
    {
        Emit::Error::undefinedMain();
    }
}

void Semantics::analyzeTree(Node *node)
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

    // Analyze children
    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        analyzeTree(children[i]);
    }

    // Leave the scope

    // Analyze sibling
    analyzeTree(node->getSibling());
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

            if (isValidMainFunc(funcNode))
            {
                m_validMainExists = true;
            }

            m_symTable->enter(funcNode->getStringValue());
            break;
        }
        case Node::DeclKind::Parm:
        {
            Parm *parmNode = (Parm *)node;
            m_symTable->insert(parmNode->getStringValue(), parmNode);
            break;
        }
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
        // Not analyzed
        case Node::StmtKind::Break:
        {
            break;
        }
        case Node::StmtKind::Compound:
        {
            m_symTable->enter("Compound");
            break;
        }
        case Node::StmtKind::For:
        {
            m_symTable->enter("For Loop");
            break;
        }
        // Not analyzed
        case Node::StmtKind::If:
        {
            break;
        }
        case Node::StmtKind::Return:
        {
            Return *returnNode = (Return *)node;
            Var *varNode = (Var *)(returnNode->getChildren()[0]);
            if (varNode != nullptr && varNode->getIsArray())
            {
                Emit::Error::generic(returnNode->getTokenLineNum(), "Cannot return an array.");
            }
            break;
        }
        // Not analyzed
        case Node::StmtKind::While:
        {
            break;
        }
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
        {
            Asgn *asgnNode = (Asgn *)node;
            break;
        }
        case Node::ExpKind::Binary:
        {
            Binary *binaryNode = (Binary *)node;
            break;
        }
        case Node::ExpKind::Call:
        {
            Call *callNode = (Call *)node;
            break;
        }
        // Not analyzed
        case Node::ExpKind::Const:
        {
            break;
        }
        case Node::ExpKind::Id:
        {
            Id *idNode = (Id *)node;
            break;
        }
        // Not analyzed
        case Node::ExpKind::Range:
        {
            break;
        }
        case Node::ExpKind::Unary:
        {
            Unary *unaryNode = (Unary *)node;
            break;
        }
        case Node::ExpKind::UnaryAsgn:
        {
            UnaryAsgn *unaryAsgnNode = (UnaryAsgn *)node;
            break;
        }
        case Node::ExpKind::ExpNone:
            throw std::runtime_error("Cannot analyze node of \'ExpNone\' \'Exp\' kind");
            break;
        default:
            throw std::runtime_error("Cannot analyze node of unknown \'Exp\' kind");
            break;
    }
}

bool Semantics::isValidMainFunc(Func *funcNode) const
{
    // Function name is not main
    if (funcNode->getStringValue() != "main")
    {
        return false;
    }

    // Is not at the top of the scope stack
    if (m_symTable->depth() != 1)
    {
        return false;
    }

    // Return type is not void
    if (funcNode->getPrimitiveType() != Primitive::Type::Void)
    {
        return false;
    }

    // Get the function children
    std::vector<Node *> funcChildren = funcNode->getChildren();

    // There are parms (children)
    if (funcChildren[0] != nullptr)
    {
        return false;
    }

    // Get the function contents
    Compound *funcCompound = (Compound *)funcChildren[1];
    std::vector<Node *> funcCompoundChildren = funcCompound->getChildren();

    // If the function is empty (main can't be empty in c-)
    if (funcCompoundChildren[0] == nullptr || funcCompoundChildren[1] == nullptr)
    {
        return false;
    }

    return true;
}