#include "Semantics.hpp"

Semantics::Semantics(SymTable *symTable) : m_symTable(symTable)
{

}

void Semantics::analyze(Node *node)
{
    analyzeTree(node);

    // A function named 'main()' must be defined
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

    // Leave the scope for every scope that was previously entered
    if (isFuncNode(node))
    {
        leaveScope();
    }
    else if (isForNode(node))
    {
        leaveScope();
    }
    else if(isCompoundNode(node))
    {
        // Don't leave the scope if the parent is a func or for
        Stmt *stmtNode = (Stmt *)node;
        if (!isFuncNode(stmtNode->getParent()) && !isForNode(stmtNode->getParent()))
        {
            leaveScope();
        }
    }

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

            m_symTable->enter("Function: " + funcNode->getName());
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
    std::vector<Node *> stmtChildren = stmtNode->getChildren();
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
            if (!isFuncNode(stmtNode->getParent()) && !isForNode(stmtNode->getParent()))
            {
                m_symTable->enter("Compound Statement");
            }
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
            if (stmtChildren.size() > 0)
            {
                Exp *returnNodeChild = (Exp *)(stmtChildren[0]);
                if (isIdNode(returnNodeChild))
                {
                    Id *idNode = (Id *)returnNodeChild;
                    if (idNode->getIsArray())
                    {
                        Emit::Error::generic(returnNode->getLineNum(), "Cannot return an array.");
                    }
                }
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
    std::vector<Node *> expChildren = expNode->getChildren();
    switch (expNode->getExpKind())
    {
        case Exp::Kind::Asgn:
        {
            // If the lhs is an id, it must have been declared
            if (isIdNode(expChildren[0]))
            {
                Id *lhsIdNode = (Id *)(expChildren[0]);
                if (!isDeclaredId(lhsIdNode))
                {
                    Emit::Error::generic(lhsIdNode->getLineNum(), "Symbol \'" + lhsIdNode->getName() + "\' is not declared.");
                }
            }

            // If the rhs is an id, it must have been declared
            if (isIdNode(expChildren[1]))
            {
                Id *rhsIdNode = (Id *)(expChildren[1]);
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
            switch (binaryNode->getType())
            {
                case Binary::Type::Mul:
                case Binary::Type::Div:
                case Binary::Type::Mod:
                case Binary::Type::Add:
                case Binary::Type::Sub:
                {
                    // Get LHS children[0] and get RHS children[1]
                    // checkAreSameType(children[0], children[1])
                    // check that the datatypes are right on both sides
                    // check that both sides have the same datatype
                    break;
                }
                case Binary::Type::Index:
                {
                    if (expChildren.size() < 2 || expChildren[0] == nullptr || expChildren[1] == nullptr)
                    {
                        throw std::runtime_error("Cannot analyze Index node of \'Binary\' kind as children are not found");
                    }

                    if (!isIdNode(expChildren[0]))
                    {
                        throw std::runtime_error("Cannot analyze Index node of \'Binary\' kind as the first child is not an Id node");
                    }

                    Id *lhsIdNode = (Id *)(expChildren[0]);
                    if (lhsIdNode->getIsArray())
                    {
                        Emit::Error::generic(expNode->getLineNum(), "Cannot index nonarray '" + lhsIdNode->getName() + "'.");
                    }
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
            Decl *prevDeclNode = (Decl *)(m_symTable->lookup(callNode->getName()));

            // If the function name is not in the symbol table
            if (prevDeclNode == nullptr)
            {
                Emit::Error::generic(expNode->getLineNum(), "Symbol '" + callNode->getName() + "' is not declared.");
                return;
            }

            // If the function name is not associated with a function
            if (!isFuncNode(prevDeclNode))
            {
                Emit::Error::generic(expNode->getLineNum(), "'" + callNode->getName() + "' is a simple variable and cannot be called.");
                if (isVarNode(prevDeclNode))
                {
                    Var *varNode = (Var *)prevDeclNode;
                    varNode->makeUsed();
                }
                else if (isParmNode(prevDeclNode))
                {
                    Parm *parmNode = (Parm *)prevDeclNode;
                    parmNode->makeUsed();
                }
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
            Decl *prevDeclNode = (Decl *)(m_symTable->lookup(idNode->getName()));
            if (prevDeclNode == nullptr)
            {
                Emit::Error::generic(idNode->getLineNum(), "Symbol '" + idNode->getName() + "' is not declared.");
                return;
            }

            if (isFuncNode(prevDeclNode))
            {
                Emit::Error::generic(idNode->getLineNum(), "Cannot use function '" + idNode->getName() + "' as a variable.");
            }
            else if (isVarNode(prevDeclNode))
            {
                Var *varNode = (Var *)prevDeclNode;
                varNode->makeUsed();
            }
            else if (isParmNode(prevDeclNode))
            {
                Parm *parmNode = (Parm *)prevDeclNode;
                parmNode->makeUsed();
            }

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
    std::map<std::string, void *> syms = m_symTable->getSyms();
    for (auto const& [name, voidNode] : syms)
    {
        Node *node = (Node *)voidNode;
        if (!isDeclNode(node))
        {
            throw std::runtime_error("Failed to check if non-Decl node causes an unused var warning");
        }

        Decl *declNode = (Decl *)node;
        if (isVarNode(declNode))
        {
            Var *varNode = (Var *)declNode;
            if (varNode->getIsUsed() == false)
            {
                Emit::Warn::generic(varNode->getLineNum(), "The variable '" + varNode->getName() + "' seems not to be used.");
            }
        }
        else if (isParmNode(declNode))
        {
            Parm *parmNode = (Parm *)declNode;
            if (parmNode->getIsUsed() == false)
            {
                Emit::Warn::generic(parmNode->getLineNum(), "The variable '" + parmNode->getName() + "' seems not to be used.");
            }
        }
    }

    m_symTable->leave();
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
                        Decl *prevDeclNode = (Decl *)(m_symTable->lookup(declNode->getName()));
                        if (prevDeclNode == nullptr)
                        {
                            throw std::runtime_error("Failed to insert \'Decl\' node and it was not already in the symbol table");
                        }
                        std::stringstream msg;
                        msg << "Symbol '" << declNode->getName() << "' is already declared at line " << prevDeclNode->getLineNum() << ".";
                        Emit::Error::generic(declNode->getLineNum(), msg.str());
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

bool Semantics::isDeclNode(const Node *node) const
{
    if (node == nullptr || node->getNodeKind() != Node::Kind::Decl)
    {
        return false;
    }
    return true;
}

bool Semantics::isFuncNode(const Node *node) const
{
    if (!isDeclNode(node))
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

bool Semantics::isParmNode(const Node *node) const
{
    if (!isDeclNode(node))
    {
        return false;
    }
    Decl *declNode = (Decl *)node;
    if (declNode->getDeclKind() != Decl::Kind::Parm)
    {
        return false;
    }
    return true;
}

bool Semantics::isVarNode(const Node *node) const
{
    if (!isDeclNode(node))
    {
        return false;
    }
    Decl *declNode = (Decl *)node;
    if (declNode->getDeclKind() != Decl::Kind::Var)
    {
        return false;
    }
    return true;
}

bool Semantics::isExpNode(const Node *node) const
{
    if (node == nullptr || node->getNodeKind() != Node::Kind::Exp)
    {
        return false;
    }
    return true;
}

bool Semantics::isIdNode(const Node *node) const
{
    if (!isExpNode(node))
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

bool Semantics::isStmtNode(const Node *node) const
{
    if (node == nullptr || node->getNodeKind() != Node::Kind::Stmt)
    {
        return false;
    }
    return true;
}

bool Semantics::isCompoundNode(const Node *node) const
{
    if (!isStmtNode(node))
    {
        return false;
    }
    Stmt *stmtNode = (Stmt *)node;
    if (stmtNode->getStmtKind() != Stmt::Kind::Compound)
    {
        return false;
    }
    return true;
}

bool Semantics::isForNode(const Node *node) const
{
    if (!isStmtNode(node))
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

bool Semantics::isValidMainFunc(const Func *funcNode) const
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

bool Semantics::isDeclaredId(const Id *idNode) const
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
