#include "Semantics.hpp"

Semantics::Semantics(SymTable *symTable) : m_symTable(symTable)
{

}

void Semantics::analyze(Node *node)
{
    analyzeTree(node);

    //  A function named 'main()' must be defined
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
        case Node::Kind::Decl:
            analyzeDecl(node);
            break;
        case Node::Kind::Stmt:
            analyzeStmt(node);
            break;
        case Node::Kind::Exp:
            analyzeExp(node);
            break;
        case Node::Kind::None:
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

    Decl *expNode = (Decl *)node;
    switch (expNode->getDeclKind())
    {
        case Decl::Kind::Func:
        {
            Func *funcNode = (Func* )expNode;
            addToSymTable(funcNode);

            if (isValidMainFunc(funcNode))
            {
                m_validMainExists = true;
            }

            m_symTable->enter(funcNode->getName());
            break;
        }
        case Decl::Kind::Parm:
        {
            Parm *parmNode = (Parm *)expNode;
            addToSymTable(parmNode);
            break;
        }
        case Decl::Kind::Var:
        {
            Var *varNode = (Var* )expNode;

            // Global vars are always initialized
            if (m_symTable->depth() == 1)
            {
                varNode->makeInitialized();
            }

            // If the variable is static, use global scope
            if (varNode->getData()->getIsStatic())
            {
                addToSymTable(varNode, true);
            }
            else
            {
                addToSymTable(varNode);
            }
            break;
        }
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

    Stmt *stmtNode = (Stmt *)node;
    switch (stmtNode->getStmtKind())
    {
        // Not analyzed
        case Stmt::Kind::Break:
        {
            break;
        }
        case Stmt::Kind::Compound:
        {
            // Ignore compounds following func or for
            Node *prevChild = stmtNode->getPreviousChild();
            if (isFuncNode(prevChild) || isForNode(prevChild))
            {
                return;
            }
            m_symTable->enter("Compound");
            break;
        }
        case Stmt::Kind::For:
        {
            m_symTable->enter("For Loop");
            break;
        }
        // Not analyzed
        case Stmt::Kind::If:
        {
            break;
        }
        case Stmt::Kind::Return:
        {
            Return *returnNode = (Return *)stmtNode;
            Var *varNode = (Var *)(returnNode->getChildren()[0]);
            if (varNode != nullptr && varNode->getData()->getIsArray())
            {
                Emit::Error::generic(returnNode->getLineNum(), "Cannot return an array.");
            }
            break;
        }
        // Not analyzed
        case Stmt::Kind::While:
        {
            break;
        }
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

    Exp *expNode = (Exp *)node;
    switch (expNode->getExpKind())
    {
        case Exp::Kind::Asgn:
        {
            Asgn *asgnNode = (Asgn *)expNode;
            std::vector<Node *> asgnChildren = asgnNode->getChildren();

            // Check if the LHS is declared, if the LHS is an Id
            if (isIdNode(asgnChildren[0]))
            {
                Id *lhsIdNode = (Id *)(asgnChildren[0]);
                if (!isDeclaredId(lhsIdNode))
                {
                    Emit::Error::generic(lhsIdNode->getLineNum(), "Symbol \'" + lhsIdNode->getName() + "\' is not declared.");
                }
            }

            // Check if the RHS is declared, if the RHS is an Id
            if (isIdNode(asgnChildren[1]))
            {
                Id *rhsIdNode = (Id *)(asgnChildren[1]);
                if (!isDeclaredId(rhsIdNode))
                {
                    Emit::Error::generic(rhsIdNode->getLineNum(), "Symbol \'" + rhsIdNode->getName() + "\' is not declared.");
                }
            }
            // Need to check LHS and RHS are the same type
            break;
        }
        case Exp::Kind::Binary:
        {
            Binary *binaryNode = (Binary *)expNode;
            std::vector<Node *> binaryChildren = binaryNode->getChildren();
            switch (binaryNode->getType())
            {
                case Binary::Type::Mul:
                case Binary::Type::Div:
                case Binary::Type::Mod:
                case Binary::Type::Add:
                case Binary::Type::Sub:
                    break;
                case Binary::Type::Index:
                {
                    if (binaryChildren[0] == nullptr || binaryChildren[1] == nullptr)
                    {
                        throw std::runtime_error("Cannot analyze Index node of \'Binary\' kind as children are not found");
                    }

                    Id *lhsExpNode = (Id *)(binaryChildren[0]);
                    if (lhsExpNode->getIsArray())
                    {
                        Emit::Error::generic(expNode->getLineNum(), "Cannot index nonarray '" + lhsExpNode->getName() + "'.");
                    }
                    // Decl *prevDeclare = (Decl *)(m_symTable->lookup(lhsExpNode->getName()));

                    // If the id is not declared
                    // if (prevDeclare == nullptr)
                    // {

                    // }

                    // Exp *rhsExpNode = (Exp *)(binaryChildren[1]);
                    break;
                }
                case Binary::Type::And:
                case Binary::Type::Or:
                    break;
                case Binary::Type::LT:
                case Binary::Type::LEQ:
                case Binary::Type::GT:
                case Binary::Type::GEQ:
                case Binary::Type::EQ:
                case Binary::Type::NEQ:
                    break;
            }
            break;
        }
        case Exp::Kind::Call:
        {
            Call *callNode = (Call *)expNode;
            Decl *declNode = (Decl *)(m_symTable->lookup(callNode->getName()));

            // If the function name is not in the symbol table
            if (declNode == nullptr)
            {
                Emit::Error::generic(expNode->getLineNum(), "Symbol '" + callNode->getName() + "' is not declared.");
                return;
            }

            // If the function name is not associated with a function
            if (declNode->getDeclKind() != Decl::Kind::Func)
            {
                Emit::Error::generic(expNode->getLineNum(), "'" + callNode->getName() + "' is a simple variable and cannot be called.");
                return;
            }
            break;
        }
        // Not analyzed
        case Exp::Kind::Const:
        {
            break;
        }
        case Exp::Kind::Id:
        {
            Id *idNode = (Id *)expNode;
            break;
        }
        // Not analyzed
        case Exp::Kind::Range:
        {
            break;
        }
        case Exp::Kind::Unary:
        {
            Unary *unaryNode = (Unary *)expNode;
            break;
        }
        case Exp::Kind::UnaryAsgn:
        {
            UnaryAsgn *unaryAsgnNode = (UnaryAsgn *)expNode;
            break;
        }
        default:
            throw std::runtime_error("Cannot analyze node of unknown \'Exp\' kind");
            break;
    }
}

void Semantics::leaveScope()
{

}

bool Semantics::addToSymTable(const Node *node, const bool global)
{
    bool inserted = false;
    if (node == nullptr)
    {
        return inserted;
    }

    switch (node->getNodeKind())
    {
        case Node::Kind::Decl:
        {
            Decl *declNode = (Decl *)node;
            switch (declNode->getDeclKind())
            {
                case Decl::Kind::Func:
                case Decl::Kind::Parm:
                case Decl::Kind::Var:
                {
                    inserted = m_symTable->insert(declNode->getName(), declNode);
                    if (!inserted)
                    {
                        Decl *prevDeclare = (Decl *)(m_symTable->lookup(declNode->getName()));
                        if (prevDeclare == nullptr)
                        {
                            throw std::runtime_error("Failed to insert \'Decl\' node and it was not already in the symbol table");
                        }
                        std::stringstream msg;
                        msg << "Symbol '" << declNode->getName() << "' is already declared at line " << prevDeclare->getLineNum() << ".";
                        Emit::Error::generic(prevDeclare->getLineNum(), msg.str());
                    }
                    break;
                }
            }
            break;
        }
        case Node::Kind::Exp:
        {
            Exp *expNode = (Exp *)node;
            switch (expNode->getExpKind())
            {
                case Exp::Kind::Asgn:
                case Exp::Kind::Binary:
                case Exp::Kind::Call:
                case Exp::Kind::Const:
                case Exp::Kind::Id:
                case Exp::Kind::Range:
                case Exp::Kind::Unary:
                case Exp::Kind::UnaryAsgn:
                    throw std::runtime_error("Cannot add \'Exp\' node to the symbol table");
                    break;
            }
            break;
        }
        case Node::Kind::Stmt:
        {
            Stmt *stmtNode = (Stmt *)node;
            switch (stmtNode->getStmtKind())
            {
                case Stmt::Kind::Break:
                case Stmt::Kind::Compound:
                case Stmt::Kind::For:
                case Stmt::Kind::If:
                case Stmt::Kind::Return:
                case Stmt::Kind::While:
                    throw std::runtime_error("Cannot add \'Stmt\' node to the symbol table");
                    break;
            }
            break;
        }
        default:
            throw std::runtime_error("Cannot add node of unknown kind to the symbol table");
            break;
    }

    return inserted;
}

bool Semantics::isFuncNode(Node *node) const
{
    if (node == nullptr || node->getNodeKind() != Node::Kind::Decl)
    {
        return false;
    }
    Decl *declNode = (Decl *)node;
    if (declNode->getDeclKind() != Decl::Kind::Func)
    {
        return false;
    }
    return true;
}

bool Semantics::isForNode(Node *node) const
{
    if (node == nullptr || node->getNodeKind() != Node::Kind::Stmt)
    {
        return false;
    }
    Stmt *stmtNode = (Stmt *)node;
    if (stmtNode->getStmtKind() != Stmt::Kind::For)
    {
        return false;
    }
    return true;
}

bool Semantics::isIdNode(Node *node) const
{
    if (node == nullptr || node->getNodeKind() != Node::Kind::Exp)
    {
        return false;
    }
    Exp *expNode = (Exp *)node;
    if (expNode->getExpKind() != Exp::Kind::Id)
    {
        return false;
    }
    return true;
}

bool Semantics::isValidMainFunc(Func *funcNode) const
{
    if (!isFuncNode(funcNode))
    {
        throw std::runtime_error("Cannot determine valid main status of non-Func or nullptr node");
    }

    // Function name is not main
    if (funcNode->getName() != "main")
    {
        return false;
    }

    // Is not at the top of the scope stack
    if (m_symTable->depth() != 1)
    {
        return false;
    }

    // Return type is not void
    if (funcNode->getData()->getType() != Data::Type::Void)
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
    if (!isIdNode(idNode))
    {
        throw std::runtime_error("Cannot determine declaration status of non-Id or nullptr node");
    }

    // If the id name is not in the symbol table, it is not declared
    if (m_symTable->lookup(idNode->getName()) == nullptr)
    {
        return false;
    }
    return true;
}
