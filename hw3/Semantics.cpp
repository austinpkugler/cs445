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
            std::vector<Node *> asgnChildren = asgnNode->getChildren();
            

            // Check if the LHS is declared, if the LHS is an Id
            if (isValidIdNode(asgnChildren[0]))
            {
                Id *lhsIdNode = (Id *)(asgnChildren[0]);
                if (!isDeclaredId(lhsIdNode))
                {
                    Emit::Error::generic(lhsIdNode->getTokenLineNum(), "Symbol \'" + lhsIdNode->getStringValue() + "\' is not declared.");
                }
            }

            // Check if the RHS is declared, if the RHS is an Id
            if (isValidIdNode(asgnChildren[1]))
            {
                Id *rhsIdNode = (Id *)(asgnChildren[1]);
                if (!isDeclaredId(rhsIdNode))
                {
                    Emit::Error::generic(rhsIdNode->getTokenLineNum(), "Symbol \'" + rhsIdNode->getStringValue() + "\' is not declared.");
                }
            }
            // Need to check LHS and RHS are the same type
            break;
        }
        case Node::ExpKind::Binary:
        {
            Binary *binaryNode = (Binary *)node;
            std::vector<Node *> binaryChildren = binaryNode->getChildren();
            switch(binaryNode->getType())
            {
                case Binary::Type::Add:
                {
                    // If the the LHS is an Id
                    if (isValidIdNode(binaryChildren[0]))
                    {

                    }
                    // std::cout << "Add lhs -- " << "getTokenLineNum()=" << binaryChildren[0]->getTokenLineNum() << " getNodeKind()=" << binaryChildren[0]->getNodeKind() << " getExpKind()=" << binaryChildren[0]->getExpKind() << " getStringValue()=" << binaryChildren[0]->getStringValue() << std::endl;
                    break;
                }
            }
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

bool Semantics::isValidIdNode(Node *node) const
{
    if (node == nullptr || node->getNodeKind() != Node::NodeKind::Exp || node->getExpKind() != Node::ExpKind::Id)
    {
        return false;
    }
    return true;
}

bool Semantics::isValidFuncNode(Node *node) const
{
    if (node == nullptr || node->getNodeKind() != Node::NodeKind::Decl || node->getDeclKind() != Node::DeclKind::Func)
    {
        return false;
    }
    return true;
}

bool Semantics::isValidMainFunc(Func *funcNode) const
{
    if (!isValidFuncNode(funcNode))
    {
        throw std::runtime_error("Cannot determine valid main status of non-Func or nullptr node");
    }

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
    if (funcNode->getPrimType() != Prim::Type::Void)
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

bool Semantics::isDeclaredId(Id *idNode) const
{
    if (!isValidIdNode(idNode))
    {
        throw std::runtime_error("Cannot determine declaration status of non-Id or nullptr node");
    }

    // If the id name is not in the symbol table
    if (m_symTable->lookup(idNode->getStringValue()) == nullptr)
    {
        return false;
    }
    return true;
}
