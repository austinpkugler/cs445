#include "Semantics.hpp"

Semantics::Semantics(SymTable *symTable) : m_symTable(symTable)
{

}

void Semantics::analyze(const Node *node)
{
    analyzeTree(node);

    // A function named 'main()' must be defined
    if (!m_validMainExists)
    {
        Emit::Error::undefinedMain();
    }
}

void Semantics::analyzeTree(const Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    switch (node->getNodeKind())
    {
        case Node::Kind::Decl:
        {
            Decl *decl = (Decl *)node;
            analyzeDecl(decl);
            break;
        }
        case Node::Kind::Exp:
        {
            Exp *exp = (Exp *)node;
            analyzeExp(exp);
            break;
        }
        case Node::Kind::Stmt:
        {
            Stmt *stmt = (Stmt *)node;
            analyzeStmt(stmt);
            break;
        }
        case Node::Kind::None:
            throw std::runtime_error("Semantics::analyzeTree() - None Node");
            break;
        default:
            throw std::runtime_error("Semantics::analyzeTree() - Unknown Node");
            break;
    }

    // Analyze children
    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        analyzeTree(children[i]);
    }

    // Leave the scope for every scope that was previously entered
    if (isFunc(node))
    {
        leaveScope();
    }
    else if (isFor(node))
    {
        leaveScope();
    }
    else if (isCompound(node))
    {
        // Don't leave the scope if the parent is a func or for
        Stmt *stmt = (Stmt *)node;
        if (!isFunc(stmt->getParent()) && !isFor(stmt->getParent()))
        {
            leaveScope();
        }
    }

    // Analyze sibling
    analyzeTree(node->getSibling());
}

void Semantics::analyzeDecl(const Decl *decl)
{
    if (!isDecl(decl))
    {
        throw std::runtime_error("Semantics::analyzeDecl() - Invalid Decl");
    }

    switch (decl->getDeclKind())
    {
        case Decl::Kind::Func:
        {
            Func *func = (Func* )decl;
            analyzeFunc(func);
            break;
        }
        case Decl::Kind::Parm:
        {
            Parm *parm = (Parm *)decl;
            analyzeParm(parm);
            break;
        }
        case Decl::Kind::Var:
        {
            Var *var = (Var* )decl;
            analyzeVar(var);
            break;
        }
        default:
            throw std::runtime_error("Semantics::analyzeDecl() - Unknown Decl");
            break;
    }
}

void Semantics::analyzeFunc(const Func *func)
{
    if (!isFunc(func))
    {
        throw std::runtime_error("Semantics::analyzeFunc() - Invalid Func");
    }

    addToSymTable(func);

    if (isValidMainFunc(func))
    {
        m_validMainExists = true;
    }

    m_symTable->enter("Function: " + func->getName());
}

void Semantics::analyzeParm(const Parm *parm)
{
    if (!isParm(parm))
    {
        throw std::runtime_error("Semantics::analyzeParm() - Invalid Parm");
    }

    addToSymTable(parm);
}

void Semantics::analyzeVar(Var *var)
{
    if (!isVar(var))
    {
        throw std::runtime_error("Semantics::analyzeVar() - Invalid Var");
    }

    // Global vars are always initialized
    if (m_symTable->depth() == 1)
    {
        var->makeInitialized();
    }

    // If the variable is static, use global scope
    // if (var->getData()->getIsStatic())
    // {
    //     addToSymTable(var, true);
    // }
    // else
    // {
    //     addToSymTable(var);
    // }
    addToSymTable(var);
}

bool Semantics::isDecl(const Node *node) const
{
    if (node == nullptr || node->getNodeKind() != Node::Kind::Decl)
    {
        return false;
    }
    return true;
}

bool Semantics::isFunc(const Node *node) const
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

bool Semantics::isParm(const Node *node) const
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

bool Semantics::isVar(const Node *node) const
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

void Semantics::analyzeExp(Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::analyzeExp() - Invalid Exp");
    }

    switch (exp->getExpKind())
    {
        case Exp::Kind::Asgn:
        {
            Asgn *asgn = (Asgn *)exp;
            analyzeAsgn(asgn);
            break;
        }
        case Exp::Kind::Binary:
        {
            Binary *binary = (Binary *)exp;
            analyzeBinary(binary);
            break;
        }
        case Exp::Kind::Call:
        {
            Call *call = (Call *)exp;
            analyzeCall(call);
            break;
        }
        case Exp::Kind::Const:
            // Not analyzed
            break;
        case Exp::Kind::Id:
        {
            Id *id = (Id *)exp;
            analyzeId(id);
            break;
        }
        case Exp::Kind::Unary:
        {
            Unary *unary = (Unary *)exp;
            analyzeUnary(unary);
            break;
        }
        case Exp::Kind::UnaryAsgn:
        {
            UnaryAsgn *unaryAsgn = (UnaryAsgn *)exp;
            analyzeUnaryAsgn(unaryAsgn);
            break;
        }
        default:
            throw std::runtime_error("Semantics::analyzeExp() - Unknown Exp");
            break;
    }
}

void Semantics::analyzeAsgn(const Asgn *asgn) const
{
    if (!isAsgn(asgn))
    {
        throw std::runtime_error("Semantics::analyzeAsgn() - Invalid Asgn");
    }

    // If the lhs is an id, it must have been declared
    std::vector<Node *> children = asgn->getChildren();
    if (isId(children[0]))
    {
        Id *lhsId = (Id *)(children[0]);
        if (!isDeclaredId(lhsId))
        {
            Emit::Error::generic(lhsId->getLineNum(), "Symbol \'" + 
    lhsId->getName() + "\' is not declared.");
        }
        Var *prevDeclLhsVar = (Var *)(m_symTable->lookup
    (lhsId->getName()));
        if (isVar(prevDeclLhsVar))
        {
            prevDeclLhsVar->makeInitialized();
        }
    }
    else
    {
        Binary *lhsBinary = (Binary *)(children[0]);
        Id *arrayId = (Id *)(lhsBinary->getChildren()[0]);
        Var *prevDeclArrayVar = (Var *)(m_symTable->lookup
    (arrayId->getName()));
        if (prevDeclArrayVar != nullptr)
        {
            prevDeclArrayVar->makeInitialized();
        }
    }
    // If the rhs is an id, it must have been declared
    if (isId(children[1]))
    {
        Id *rhsId = (Id *)(children[1]);
        if (!isDeclaredId(rhsId))
        {
            Emit::Error::generic(rhsId->getLineNum(), "Symbol \'" + 
    rhsId->getName() + "\' is not declared.");
        }
    }
    // Need to check LHS and RHS are the same type
}

void Semantics::analyzeBinary(const Binary *binary) const
{
    if (!isBinary(binary))
    {
        throw std::runtime_error("Semantics::analyzeBinary() - Invalid Binary");
    }

    checkOperandTypes(binary);
}

void Semantics::analyzeCall(const Call *call) const
{
    if (!isCall(call))
    {
        throw std::runtime_error("Semantics::analyzeCall() - Invalid Call");
    }

    Decl *prevDecl = (Decl *)(m_symTable->lookup(call->getName()));

    // If the function name is not in the symbol table
    if (prevDecl == nullptr)
    {
        Emit::Error::generic(call->getLineNum(), "Symbol '" + call->getName() + "' is not declared.");
        return;
    }

    // If the function name is not associated with a function
    if (!isFunc(prevDecl))
    {
        Emit::Error::generic(call->getLineNum(), "'" + call->getName() + "' is a simple variable and cannot be called.");
        if (isVar(prevDecl))
        {
            Var *var = (Var *)prevDecl;
            var->makeUsed();
        }
        else if (isParm(prevDecl))
        {
            Parm *parm = (Parm *)prevDecl;
            parm->makeUsed();
        }
        return;
    }
}

void Semantics::analyzeId(const Id *id) const
{
    if (!isId(id))
    {
        throw std::runtime_error("Semantics::analyzeId() - Invalid Id");
    }

    Decl *prevDecl = (Decl *)(m_symTable->lookup(id->getName()));
    if (prevDecl == nullptr)
    {
        Emit::Error::generic(id->getLineNum(), "Symbol '" + id->getName() + "' is not declared.");
        return;
    }
    if (isFunc(prevDecl))
    {
        Emit::Error::generic(id->getLineNum(), "Cannot use function '" + id->getName() + "' as a variable.");
    }
    else if (isVar(prevDecl))
    {
        Var *prevDeclVar = (Var *)prevDecl;
        prevDeclVar->makeUsed();
        if (!prevDeclVar->getIsInitialized())
        {
            Emit::Warn::generic(id->getLineNum(), "Variable '" + id->getName() + "' may be uninitialized when used here.");
        }
    }
    else if (isParm(prevDecl))
    {
        Parm *prevDeclParm = (Parm *)prevDecl;
        prevDeclParm->makeUsed();
    }
}

void Semantics::analyzeUnary(const Unary *unary) const
{
    if (!isUnary(unary))
    {
        throw std::runtime_error("Semantics::analyzeUnary() - Invalid Unary");
    }
}

void Semantics::analyzeUnaryAsgn(const UnaryAsgn *unaryAsgn) const
{
    if (!isUnaryAsgn(unaryAsgn))
    {
        throw std::runtime_error("Semantics::analyzeUnaryAsgn() - Invalid UnaryAsgn");
    }
}

bool Semantics::isExp(const Node *node) const
{
    if (node == nullptr || node->getNodeKind() != Node::Kind::Exp)
    {
        return false;
    }
    return true;
}

bool Semantics::isAsgn(const Node *node) const
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

bool Semantics::isBinary(const Node *node) const
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

bool Semantics::isCall(const Node *node) const
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

bool Semantics::isConst(const Node *node) const
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

bool Semantics::isId(const Node *node) const
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

bool Semantics::isUnary(const Node *node) const
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

bool Semantics::isUnaryAsgn(const Node *node) const
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

void Semantics::analyzeStmt(const Stmt *stmt) const
{
    if (!isStmt(stmt))
    {
        throw std::runtime_error("Semantics::analyzeStmt() - Invalid Stmt");
    }

    switch (stmt->getStmtKind())
    {
        case Stmt::Kind::Break:
            // Not analyzed
            break;
        case Stmt::Kind::Compound:
        {
            Compound *compound = (Compound *)stmt;
            analyzeCompound(compound);
            break;
        }
        case Stmt::Kind::For:
            analyzeFor();
            break;
        case Stmt::Kind::If:
            // Not analyzed
            break;
        case Stmt::Kind::Return:
        {
            Return *returnN = (Return *)stmt;
            analyzeReturn(returnN);
            break;
        }
        case Stmt::Kind::While:
        case Stmt::Kind::Range:
            // Not analyzed
            break;
        default:
            throw std::runtime_error("Semantics::analyzeStmt() - Unknown Stmt");
            break;
    }
}

void Semantics::analyzeCompound(const Compound *compound) const
{
    if (!isCompound(compound))
    {
        throw std::runtime_error("Semantics::analyzeCompound() - Invalid Compound");
    }

    // Ignore compounds following func or for
    if (!isFunc(compound->getParent()) && !isFor(compound->getParent()))
    {
        m_symTable->enter("Compound Statement");
    }
}

void Semantics::analyzeFor() const
{
    m_symTable->enter("For Loop");
}

void Semantics::analyzeReturn(const Return *returnN) const
{
    if (!isReturn(returnN))
    {
        throw std::runtime_error("Semantics::analyzeReturn() - Invalid Return");
    }

    std::vector<Node *> children = returnN->getChildren();
    if (children.size() > 0)
    {
        Exp *returnChild = (Exp *)(children[0]);
        if (isId(returnChild))
        {
            Id *id = (Id *)returnChild;
            if (id->getIsArray())
            {
                Emit::Error::generic(returnN->getLineNum(), "Cannot return an array.");
            }
        }
    }
}

bool Semantics::isStmt(const Node *node) const
{
    if (node == nullptr || node->getNodeKind() != Node::Kind::Stmt)
    {
        return false;
    }
    return true;
}

bool Semantics::isBreak(const Node *node) const
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

bool Semantics::isCompound(const Node *node) const
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

bool Semantics::isFor(const Node *node) const
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

bool Semantics::isIf(const Node *node) const
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

bool Semantics::isRange(const Node *node) const
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

bool Semantics::isReturn(const Node *node) const
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

bool Semantics::isWhile(const Node *node) const
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

bool Semantics::isValidMainFunc(const Func *func) const
{
    if (!isFunc(func))
    {
        throw std::runtime_error("Semantics::isValidMainFunc() - Invalid Func");
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
    if (!isId(id))
    {
        throw std::runtime_error("Semantics::isDeclaredId() - Invalid Id");
    }

    // If the id name is not in the symbol table, it is not declared
    if (m_symTable->lookup(id->getName()) == nullptr)
    {
        return false;
    }
    return true;
}

void Semantics::checkOperandTypes(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::checkOperandTypes() - Invalid Exp");
    }

    std::vector<Node *> expChildren = exp->getChildren();

    if (isBinary(exp))
    {
        Binary *binary = (Binary *)exp;

        if (expChildren.size() < 2 || expChildren[0] == nullptr || expChildren[1] == nullptr)
        {
            throw std::runtime_error("Semantics::checkOperandTypes() - LHS and RHS must exist");
        }

        if (!isExp(expChildren[0]) || !isExp(expChildren[1]))
        {
            throw std::runtime_error("Semantics::checkOperandTypes() - LHS and RHS must be Exp");
        }

        switch (binary->getType())
        {
            case Binary::Type::Mul:
            case Binary::Type::Div:
            case Binary::Type::Mod:
            case Binary::Type::Add:
            case Binary::Type::Sub:
                break;
            case Binary::Type::Index:
            {
                if (!isId(expChildren[0]))
                {
                    throw std::runtime_error("Semantics::checkOperandTypes() - First child is not an Id");
                }
                Id *arrayId = (Id *)(expChildren[0]);
                if (!arrayId->getIsArray())
                {
                    Emit::Error::generic(arrayId->getLineNum(), "Cannot index nonarray '" + arrayId->getName() + "'.");
                    return;
                }
                Node *arrayIndexNode = expChildren[1];
                if (isId(arrayIndexNode))
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
                throw std::runtime_error("Semantics::checkOperandTypes() - Unknown Binary");
                break;
        }
    }
}

void Semantics::leaveScope()
{
    std::map<std::string, void *> syms = m_symTable->getSyms();
    for (auto const& [name, voidNode] : syms)
    {
        Node *node = (Node *)voidNode;
        if (!isDecl(node))
        {
            throw std::runtime_error("Semantics: symbol table error: \'NonDecl\' node found in symbol table: node is \'NonDecl:{ any }\' or nullptr");
        }

        Decl *decl = (Decl *)node;
        if (isVar(decl))
        {
            Var *var = (Var *)decl;
            if (var->getIsUsed() == false)
            {
                Emit::Warn::generic(var->getLineNum(), "The variable '" + var->getName() + "' seems not to be used.");
            }
        }
        else if (isParm(decl))
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

bool Semantics::addToSymTable(const Decl *decl, const bool global)
{
    if (!isDecl(decl))
    {
        throw std::runtime_error("Semantics::addToSymTable() - Invalid Decl");
    }

    bool inserted = false;
    if (global)
    {
        inserted = m_symTable->insertGlobal(decl->getName(), (void *)decl);
    }
    else
    {
        inserted = m_symTable->insert(decl->getName(), (void *)decl);
    }

    if (!inserted)
    {
        Decl *prevDecl = (Decl *)(m_symTable->lookup(decl->getName()));
        if (prevDecl == nullptr)
        {
            throw std::runtime_error("Semantics::addToSymTable() - Failed to insert Decl");
        }
        std::stringstream msg;
        msg << "Symbol '" << decl->getName() << "' is already declared at line " << prevDecl->getLineNum() << ".";
        Emit::Error::generic(decl->getLineNum(), msg.str());
    }

    return inserted;
}

Decl * Semantics::getFromSymTable(const std::string name) const
{
    if (name.length() == 0)
    {
        throw std::runtime_error("Semantics::getFromSymTable() - Invalid name");
    }

    Decl *prevDecl = (Decl *)(m_symTable->lookup(name));
    return prevDecl;
}

Data * Semantics::setAndGetExpData(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::setAndGetExpData() - Invalid Exp");
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
