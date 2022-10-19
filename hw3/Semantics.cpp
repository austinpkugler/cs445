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
            throw std::runtime_error("Semantics: analyze error: cannot analyze \'None:{ any }\' node");
            break;
        default:
            throw std::runtime_error("Semantics: analyze error: cannot analyze \'{ any }:{ any }\' node");
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
        Stmt *stmt = (Stmt *)node;
        if (!isFuncNode(stmt->getParent()) && !isForNode(stmt->getParent()))
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
        throw std::runtime_error("Semantics: analyze error: cannot analyze \'Decl:{ any }\' node: node is nullptr");
    }

    Decl *decl = (Decl *)node;
    switch (decl->getDeclKind())
    {
        case Decl::Kind::Func:
        {
            Func *func = (Func* )decl;
            addToSymTable(func);

            if (isValidMainFunc(func))
            {
                m_validMainExists = true;
            }

            m_symTable->enter("Function: " + func->getName());
            break;
        }
        case Decl::Kind::Parm:
        {
            Parm *parm = (Parm *)decl;
            addToSymTable(parm);
            break;
        }
        case Decl::Kind::Var:
        {
            Var *var = (Var* )decl;

            // Global vars are always initialized
            if (m_symTable->depth() == 1)
            {
                var->makeInitialized();
            }

            // If the variable is static, use global scope
            if (var->getData()->getIsStatic())
            {
                addToSymTable(var, true);
            }
            else
            {
                addToSymTable(var);
            }
            break;
        }
        default:
            throw std::runtime_error("Semantics: analyze error: cannot analyze \'Decl:{ any }\' node");
            break;
    }
}

void Semantics::analyzeStmt(Node *node) const
{
    if (node == nullptr)
    {
        throw std::runtime_error("Semantics: analyze error: cannot analyze \'Stmt:{ any }\' node: node is nullptr");
    }

    Stmt *stmt = (Stmt *)node;
    std::vector<Node *> stmtChildren = stmt->getChildren();
    switch (stmt->getStmtKind())
    {
        // Not analyzed
        case Stmt::Kind::Break:
        {
            break;
        }
        case Stmt::Kind::Compound:
        {
            // Ignore compounds following func or for
            if (!isFuncNode(stmt->getParent()) && !isForNode(stmt->getParent()))
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
            Return *ret = (Return *)stmt;
            if (stmtChildren.size() > 0)
            {
                Exp *retChild = (Exp *)(stmtChildren[0]);
                if (isIdNode(retChild))
                {
                    Id *id = (Id *)retChild;
                    if (id->getIsArray())
                    {
                        Emit::Error::generic(ret->getLineNum(), "Cannot return an array.");
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
        // Not analyzed
        case Stmt::Kind::Range:
        {
            break;
        }
        default:
            throw std::runtime_error("Semantics: analyze error: cannot analyze \'Stmt:{ any }\' node");
            break;
    }
}

void Semantics::analyzeExp(Node *node) const
{
    if (node == nullptr)
    {
        throw std::runtime_error("Semantics: analyze error: cannot analyze \'Exp:{ any }\' node: node is nullptr");
    }

    Exp *exp = (Exp *)node;
    std::vector<Node *> expChildren = exp->getChildren();
    switch (exp->getExpKind())
    {
        case Exp::Kind::Asgn:
        {
            // If the lhs is an id, it must have been declared
            if (isIdNode(expChildren[0]))
            {
                Id *lhsId = (Id *)(expChildren[0]);
                if (!isDeclaredId(lhsId))
                {
                    Emit::Error::generic(lhsId->getLineNum(), "Symbol \'" + lhsId->getName() + "\' is not declared.");
                }
                Var *prevDeclLhsVar = (Var *)(m_symTable->lookup(lhsId->getName()));
                if (isVarNode(prevDeclLhsVar))
                {
                    prevDeclLhsVar->makeInitialized();
                }
            }
            else
            {
                Binary *lhsBinary = (Binary *)(expChildren[0]);
                Id *arrayId = (Id *)(lhsBinary->getChildren()[0]);
                Var *prevDeclArrayVar = (Var *)(m_symTable->lookup(arrayId->getName()));
                if (prevDeclArrayVar != nullptr)
                {
                    prevDeclArrayVar->makeInitialized();
                }
            }

            // If the rhs is an id, it must have been declared
            if (isIdNode(expChildren[1]))
            {
                Id *rhsId = (Id *)(expChildren[1]);
                if (!isDeclaredId(rhsId))
                {
                    Emit::Error::generic(rhsId->getLineNum(), "Symbol \'" + rhsId->getName() + "\' is not declared.");
                }
            }
            // Need to check LHS and RHS are the same type
            break;
        }
        case Exp::Kind::Binary:
        {
            if (expChildren.size() < 2 || expChildren[0] == nullptr || expChildren[1] == nullptr)
            {
                throw std::runtime_error("Semantics: analyze error: cannot analyze \'Binary:Op\' node: insufficient children");
            }

            if (!isExpNode(expChildren[0]) || !isExpNode(expChildren[1]))
            {
                throw std::runtime_error("Semantics: analyze error: cannot analyze \'Binary:Op\' node: lhs and rhs must be \'Exp:{ any }\'");
            }

            Binary *binary = (Binary *)exp;
            switch (binary->getType())
            {
                case Binary::Type::Mul:
                case Binary::Type::Div:
                case Binary::Type::Mod:
                case Binary::Type::Add:
                case Binary::Type::Sub:
                {
                    break;
                }
                case Binary::Type::Index:
                {
                    if (expChildren.size() < 2 || expChildren[0] == nullptr || expChildren[1] == nullptr)
                    {
                        throw std::runtime_error("Semantics: analyze error: cannot analyze \'Binary:Index\' node: insufficient children");
                    }

                    if (!isIdNode(expChildren[0]))
                    {
                        throw std::runtime_error("Semantics: analyze error: cannot analyze \'Binary:Index\' node: first child is not \'Exp:Id\' node");
                    }

                    Id *arrayId = (Id *)(expChildren[0]);
                    if (!arrayId->getIsArray())
                    {
                        Emit::Error::generic(arrayId->getLineNum(), "Cannot index nonarray '" + arrayId->getName() + "'.");
                        return;
                    }

                    Node *arrayIndexNode = expChildren[1];
                    if (isIdNode(arrayIndexNode))
                    {
                        Id *arrayIndexId = (Id *)arrayIndexNode;
                        Decl *prevDecl = (Decl *)(m_symTable->lookup(arrayIndexId->getName()));
                        if (prevDecl->getData()->getType() != Data::Type::Int)
                        {
                            Emit::Error::generic(arrayIndexNode->getLineNum(), "Array '" + arrayId->getName() + "' should be indexed by type int but got type " + prevDecl->getData()->stringify() + ".");
                        }
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
                {
                    Exp *lhsExp = (Exp *)(expChildren[0]), *rhsExp = (Exp *)(expChildren[1]);
                    Data *lhsData = setAndGetExpData(lhsExp), *rhsData = setAndGetExpData(rhsExp);
                    if (lhsData->getType() != Data::Type::None && lhsData->getType() != rhsData->getType())
                    {
                        Emit::Error::generic(lhsExp->getLineNum(), "'" + binary->stringify() + "' requires operands of the same type but lhs is " + lhsData->stringify() + " and rhs is " + rhsData->stringify() + ".");
                    }
                    break;
                }
                default:
                    throw std::runtime_error("Semantics: analyze error: cannot analyze \'Exp:Binary\' node: unknown \'Binary::Type\'");
                    break;
            }
            break;
        }
        case Exp::Kind::Call:
        {
            Call *call = (Call *)exp;
            Decl *prevDecl = (Decl *)(m_symTable->lookup(call->getName()));

            // If the function name is not in the symbol table
            if (prevDecl == nullptr)
            {
                Emit::Error::generic(exp->getLineNum(), "Symbol '" + call->getName() + "' is not declared.");
                return;
            }

            // If the function name is not associated with a function
            if (!isFuncNode(prevDecl))
            {
                Emit::Error::generic(exp->getLineNum(), "'" + call->getName() + "' is a simple variable and cannot be called.");
                if (isVarNode(prevDecl))
                {
                    Var *var = (Var *)prevDecl;
                    var->makeUsed();
                }
                else if (isParmNode(prevDecl))
                {
                    Parm *parm = (Parm *)prevDecl;
                    parm->makeUsed();
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
            Id *id = (Id *)exp;
            Decl *prevDecl = (Decl *)(m_symTable->lookup(id->getName()));
            if (prevDecl == nullptr)
            {
                Emit::Error::generic(id->getLineNum(), "Symbol '" + id->getName() + "' is not declared.");
                return;
            }

            if (isFuncNode(prevDecl))
            {
                Emit::Error::generic(id->getLineNum(), "Cannot use function '" + id->getName() + "' as a variable.");
            }
            else if (isVarNode(prevDecl))
            {
                Var *prevDeclVar = (Var *)prevDecl;
                prevDeclVar->makeUsed();
                if (!prevDeclVar->getIsInitialized())
                {
                    Emit::Warn::generic(id->getLineNum(), "Variable '" + id->getName() + "' may be uninitialized when used here.");
                }
            }
            else if (isParmNode(prevDecl))
            {
                Parm *prevDeclParm = (Parm *)prevDecl;
                prevDeclParm->makeUsed();
            }

            break;
        }
        case Exp::Kind::Unary:
        {
            Unary *unary = (Unary *)exp;
            break;
        }
        case Exp::Kind::UnaryAsgn:
        {
            UnaryAsgn *unaryAsgn = (UnaryAsgn *)exp;
            break;
        }
        default:
            throw std::runtime_error("Semantics: analyze error: cannot analyze \'Exp:{ any }\' node");
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
            throw std::runtime_error("Semantics: symbol table error: \'NonDecl\' node found in symbol table: node is \'NonDecl:{ any }\' or nullptr");
        }

        Decl *decl = (Decl *)node;
        if (isVarNode(decl))
        {
            Var *var = (Var *)decl;
            if (var->getIsUsed() == false)
            {
                Emit::Warn::generic(var->getLineNum(), "The variable '" + var->getName() + "' seems not to be used.");
            }
        }
        else if (isParmNode(decl))
        {
            Parm *parm = (Parm *)decl;
            if (parm->getIsUsed() == false)
            {
                Emit::Warn::generic(parm->getLineNum(), "The variable '" + parm->getName() + "' seems not to be used.");
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
            Decl *decl = (Decl *)node;
            switch (decl->getDeclKind())
            {
                case Decl::Kind::Func:
                case Decl::Kind::Parm:
                case Decl::Kind::Var:
                {
                    inserted = m_symTable->insert(decl->getName(), decl);
                    if (!inserted)
                    {
                        Decl *prevDecl = (Decl *)(m_symTable->lookup(decl->getName()));
                        if (prevDecl == nullptr)
                        {
                            throw std::runtime_error("Semantics: symbol table error: failed to insert \'Decl\' node and it was not already in the symbol table");
                        }
                        std::stringstream msg;
                        msg << "Symbol '" << decl->getName() << "' is already declared at line " << prevDecl->getLineNum() << ".";
                        Emit::Error::generic(decl->getLineNum(), msg.str());
                    }
                    break;
                }
                default:
                    throw std::runtime_error("Semantics: symbol table error: cannot insert \'Decl:{ any }\' node");
                    break;
            }
            break;
        }
        case Node::Kind::Exp:
        {
            Exp *exp = (Exp *)node;
            switch (exp->getExpKind())
            {
                case Exp::Kind::Asgn:
                case Exp::Kind::Binary:
                case Exp::Kind::Call:
                case Exp::Kind::Const:
                case Exp::Kind::Id:
                case Exp::Kind::Unary:
                case Exp::Kind::UnaryAsgn:
                    throw std::runtime_error("Semantics: symbol table error: cannot insert \'NonDecl\' node: node is \'Exp:{ any }\'");
                    break;
                default:
                    throw std::runtime_error("Semantics: symbol table error: cannot insert \'Exp:{ any }\' node");
                    break;
            }
            break;
        }
        case Node::Kind::Stmt:
        {
            Stmt *stmt = (Stmt *)node;
            switch (stmt->getStmtKind())
            {
                case Stmt::Kind::Break:
                case Stmt::Kind::Compound:
                case Stmt::Kind::For:
                case Stmt::Kind::If:
                case Stmt::Kind::Return:
                case Stmt::Kind::While:
                case Stmt::Kind::Range:
                    throw std::runtime_error("Semantics: symbol table error: cannot insert \'NonDecl\' node: node is \'Stmt:{ any }\'");
                    break;
                default:
                    throw std::runtime_error("Semantics: symbol table error: cannot insert \'Stmt:{ any }\' node");
                    break;
            }
            break;
        }
        case Node::Kind::None:
            throw std::runtime_error("Semantics: symbol table error: cannot insert \'NonDecl\' node: node is \'None:{ any }\' node");
            break;
        default:
            throw std::runtime_error("Semantics: symbol table error: cannot insert \'NonDecl\' node: node is \'{ any }:{ any }\' node");
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
    Decl *decl = (Decl *)node;
    if (decl->getDeclKind() != Decl::Kind::Func)
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
    Decl *decl = (Decl *)node;
    if (decl->getDeclKind() != Decl::Kind::Parm)
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
    Decl *decl = (Decl *)node;
    if (decl->getDeclKind() != Decl::Kind::Var)
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
    Exp *exp = (Exp *)node;
    if (exp->getExpKind() != Exp::Kind::Id)
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
    Stmt *stmt = (Stmt *)node;
    if (stmt->getStmtKind() != Stmt::Kind::Compound)
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
    Stmt *stmt = (Stmt *)node;
    if (stmt->getStmtKind() != Stmt::Kind::For)
    {
        return false;
    }
    return true;
}

bool Semantics::isValidMainFunc(const Func *func) const
{
    if (!isFuncNode(func))
    {
        throw std::runtime_error("Semantics: is error: cannot determine if node is valid main: node is \'{ any }:NonFunc\' node or nullptr");
    }

    // Function name is not main
    if (func->getName() != "main")
    {
        return false;
    }

    // Is not at the top of the scope stack
    if (m_symTable->depth() != 1)
    {
        return false;
    }

    // Return type is not void
    if (func->getData()->getType() != Data::Type::Void)
    {
        return false;
    }

    // Get the function children
    std::vector<Node *> funcChildren = func->getChildren();

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

bool Semantics::isDeclaredId(const Id *id) const
{
    if (!isIdNode(id))
    {
        throw std::runtime_error("Semantics: is error: cannot determine if node is valid main: node is \'{ any }:NonId\' node or nullptr");
    }

    // If the id name is not in the symbol table, it is not declared
    if (m_symTable->lookup(id->getName()) == nullptr)
    {
        return false;
    }
    return true;
}

Data * Semantics::setAndGetExpData(const Exp *exp) const
{
    if (!isExpNode(exp))
    {
        throw std::runtime_error("Semantics: helper error: cannot get \'Data\' for node: node is \'NonExp:{ any }\' node or nullptr");
    }

    std::vector<Node *> expChildren = exp->getChildren();

    std::string name;
    switch (exp->getExpKind())
    {
        case Exp::Kind::Asgn:
        {
            Asgn *asgn = (Asgn *)exp;
            Exp *lhsExp = (Exp *)expChildren[0];
            asgn->setData(setAndGetExpData(lhsExp));
            break;
        }
        case Exp::Kind::Binary:
        {
            Binary *binary = (Binary *)exp;
            if (binary->getType() == Binary::Type::Index)
            {
                Id *arrayId = (Id *)(binary->getChildren()[0]);
                binary->setData(new Data(setAndGetExpData(arrayId)->getType(), false, false));
            }
            break;
        }
        case Exp::Kind::Call:
        {
            Call *call = (Call *)exp;
            Decl *prevDecl = getFromSymTable(call->getName());
            if (prevDecl != nullptr)
            {
                call->setData(prevDecl->getData());
            }
            break;
        }
        case Exp::Kind::Id:
        {
            Id *id = (Id *)exp;
            Decl *prevDecl = getFromSymTable(id->getName());
            if (prevDecl != nullptr)
            {
                id->setData(prevDecl->getData());
            }
            break;
        }
    }
    return exp->getData();
}

Decl * Semantics::getFromSymTable(const std::string name) const
{ 
    if (name.length() == 0)
    {
        throw std::runtime_error("Semantics: helper error: cannot get \'Decl\' from sym table for name: name is the empty string");
    }
    
    Decl *prevDecl = (Decl *)(m_symTable->lookup(name));
    return prevDecl;
}


