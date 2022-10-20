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
    if (node == nullptr || node->getIsAnalyzed())
    {
        return;
    }

    node->makeAnalyzed();

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
            if (stmt->getStmtKind() == Stmt::Kind::Range)
            {
                return;
            }
            analyzeStmt(stmt);
            break;
        }
        case Node::Kind::None:
            throw std::runtime_error("Semantics::analyzeTree() - None kind");
            break;
        default:
            throw std::runtime_error("Semantics::analyzeTree() - Unknown kind");
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
            throw std::runtime_error("Semantics::analyzeDecl() - Unknown kind");
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

    if (isMainFunc(func))
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
    if (m_symTable->depth() == 1 || var->getData()->getIsStatic())
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

void Semantics::analyzeExp(Exp *exp)
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::analyzeExp() - Invalid Exp");
    }

    switch (exp->getExpKind())
    {
        case Exp::Kind::Asgn:
            analyzeAsgn((Asgn *)exp);
            break;
        case Exp::Kind::Binary:
            analyzeBinary((Binary *)exp);
            break;
        case Exp::Kind::Call:
            analyzeCall((Call *)exp);
            break;
        case Exp::Kind::Const:
            // Not analyzed
            break;
        case Exp::Kind::Id:
            analyzeId((Id *)exp);
            break;
        case Exp::Kind::Unary:
            analyzeUnary((Unary *)exp);
            break;
        case Exp::Kind::UnaryAsgn:
            analyzeUnaryAsgn((UnaryAsgn *)exp);
            break;
        default:
            throw std::runtime_error("Semantics::analyzeExp() - Unknown kind");
            break;
    }
}

void Semantics::analyzeAsgn(const Asgn *asgn)
{
    if (!isAsgn(asgn))
    {
        throw std::runtime_error("Semantics::analyzeAsgn() - Invalid Asgn");
    }

    // If the LHS is an id, it must have been declared
    std::vector<Node *> children = asgn->getChildren();
    Node *lhsNode = (Node *)(children[0]);
    Node *rhsNode = (Node *)(children[1]);

    analyzeTree(rhsNode);

    if (isId(lhsNode))
    {
        Id *lhsId = (Id *)(lhsNode);
        Var *prevDeclLhsVar = (Var *)(getFromSymTable(lhsId->getName()));
        if (isVar(prevDeclLhsVar))
        {
            prevDeclLhsVar->makeInitialized();
        }
    }
    else
    {
        Binary *lhsBinary = (Binary *)(children[0]);
        Id *arrayId = (Id *)(lhsBinary->getChildren()[0]);
        Var *prevDeclArrayVar = (Var *)(getFromSymTable(arrayId->getName()));
        if (prevDeclArrayVar != nullptr)
        {
            prevDeclArrayVar->makeInitialized();
        }
    }

    switch (asgn->getType())
    {
        case Asgn::Type::Asgn:
            checkOperandsOfSameType((Exp *)asgn);
            break;
        case Asgn::Type::AddAsgn:
        case Asgn::Type::SubAsgn:
        case Asgn::Type::DivAsgn:
        case Asgn::Type::MulAsgn:
            checkOperandsOfType((Exp *)asgn, Data::Type::Int);
            break;
        default:
            throw std::runtime_error("Semantics::analyzeAsgn() - Unknown type");
            break;
    }
}

void Semantics::analyzeBinary(const Binary *binary) const
{
    if (!isBinary(binary))
    {
        throw std::runtime_error("Semantics::analyzeBinary() - Invalid Binary");
    }
    if (!expOperandsExist((Exp *)binary))
    {
        throw std::runtime_error("Semantics::analyzeBinary() - LHS and RHS Exp operands must exist");
    }

    switch (binary->getType())
    {
        case Binary::Type::Mul:
        case Binary::Type::Div:
        case Binary::Type::Mod:
        case Binary::Type::Add:
        case Binary::Type::Sub:
            checkOperandsOfType((Exp *)binary, Data::Type::Int);
            break;
        case Binary::Type::Index:
            checkIndex(binary);
            break;
        case Binary::Type::And:
        case Binary::Type::Or:
            checkOperandsOfType((Exp *)binary, Data::Type::Bool);
            break;
        case Binary::Type::LT:
        case Binary::Type::LEQ:
        case Binary::Type::GT:
        case Binary::Type::GEQ:
        case Binary::Type::EQ:
        case Binary::Type::NEQ:
            checkOperandsOfSameType((Exp *)binary);
            break;
        default:
            throw std::runtime_error("Semantics::analyzeBinary() - Unknown type");
            break;
    }
}

void Semantics::analyzeCall(const Call *call) const
{
    if (!isCall(call))
    {
        throw std::runtime_error("Semantics::analyzeCall() - Invalid Call");
    }

    Decl *prevDecl = (Decl *)(getFromSymTable(call->getName()));

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

    Decl *prevDecl = (Decl *)(getFromSymTable(id->getName()));
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

        // Don't warn if the uninitialized id is an array index (see hw4/test/lhs.c-)
        if (!prevDeclVar->getIsInitialized() && prevDeclVar->getShowErrors())
        {
            if (!isIndex((Exp *)id) || id->getIsArray())
            {
                Emit::Warn::generic(id->getLineNum(), "Variable '" + id->getName() + "' may be uninitialized when used here.");
                prevDeclVar->setShowErrors(false);
            }
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

    checkUnaryOperands(unary);
}

void Semantics::analyzeUnaryAsgn(const UnaryAsgn *unaryAsgn) const
{
    if (!isUnaryAsgn(unaryAsgn))
    {
        throw std::runtime_error("Semantics::analyzeUnaryAsgn() - Invalid UnaryAsgn");
    }

    checkUnaryAsgnOperands(unaryAsgn);
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
            Return *isReturn = (Return *)stmt;
            analyzeReturn(isReturn);
            break;
        }
        case Stmt::Kind::While:
            break;
        case Stmt::Kind::Range:
            // Not analyzed
            throw std::runtime_error("Semantics::analyzeStmt() - Range kind");
            break;
        default:
            throw std::runtime_error("Semantics::analyzeStmt() - Unknown kind");
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
            Decl *prevDecl = (Decl *)(getFromSymTable(id->getName()));
            if ((prevDecl != nullptr && prevDecl->getData()->getIsArray()) || id->getIsArray())
            {
                Emit::Error::generic(returnN->getLineNum(), "Cannot return an array.");
            }
        }
    }
}

void Semantics::checkOperandsOfSameType(Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::checkOperandsOfSameType() - Invalid Exp");
    }
    if (!expOperandsExist(exp))
    {
        throw std::runtime_error("Semantics::checkOperandsOfSameType() - LHS and RHS Exp operands must exist");
    }

    std::string sym = getExpSym(exp);

    std::vector<Node *> children = exp->getChildren();
    Exp *lhsExp = (Exp *)(children[0]);
    Exp *rhsExp = (Exp *)(children[1]);
    Data *lhsData = setAndGetExpData(lhsExp);
    Data *rhsData = setAndGetExpData(rhsExp);

    // Ignore cases where the LHS has no type
    if (lhsData->getType() == Data::Type::None)
    {
        return;
    }

    // Both sides must be the same type
    if (lhsData->getType() != rhsData->getType())
    {
        Emit::Error::generic(exp->getLineNum(), "'" + sym + "' requires operands of the same type but lhs is type " + lhsData->stringify() + " and rhs is type " + rhsData->stringify() + ".");
    }

    // Both sides must be arrays or both must not be arrays
    if (lhsData->getIsArray() && !rhsData->getIsArray())
    {
        Emit::Error::generic(exp->getLineNum(), "'" + sym + "' requires both operands be arrays or not but lhs is an array and rhs is not an array.");
    }
    if (!lhsData->getIsArray() && rhsData->getIsArray())
    {
        Emit::Error::generic(exp->getLineNum(), "'" + sym + "' requires both operands be arrays or not but lhs is not an array and rhs is an array.");
    }
}

void Semantics::checkOperandsOfType(Exp *exp, const Data::Type type) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::checkOperandsOfSameType() - Invalid Exp");
    }
    if (!expOperandsExist(exp))
    {
        throw std::runtime_error("Semantics::checkOperandsOfSameType() - LHS and RHS Exp operands must exist");
    }

    std::string sym = getExpSym(exp);
    std::string typeString = Data::stringifyType(type);

    std::vector<Node *> children = exp->getChildren();
    Exp *lhsExp = (Exp *)(children[0]);
    Exp *rhsExp = (Exp *)(children[1]);
    Data *lhsData = setAndGetExpData(lhsExp);
    Data *rhsData = setAndGetExpData(rhsExp);

    // Ignore cases where the LHS or RHS has no type
    if (lhsData->getType() == Data::Type::None || rhsData->getType() == Data::Type::None)
    {
        return;
    }

    if (lhsData->getType() != type)
    {
        Emit::Error::generic(exp->getLineNum(), "'" + sym + "' requires operands of type " + typeString + " but lhs is of type " + lhsData->stringify() + ".");
    }

    if (rhsData->getType() != type)
    {
        Emit::Error::generic(exp->getLineNum(), "'" + sym + "' requires operands of type " + typeString + " but rhs is of type " + rhsData->stringify() + ".");
    }

    // If it is a binary operation, we want the operands to be only the passed type (not an array of that type)
    if (isBinary(exp))
    {
        Binary *binary = (Binary *)exp;
        if (isId(lhsExp))
        {
            Id *lhsId = (Id *)lhsExp;
            Decl *prevDecl = (Decl *)(getFromSymTable(lhsId->getName()));
            if ((prevDecl != nullptr && prevDecl->getData()->getIsArray()) || lhsId->getIsArray())
            {
                Emit::Error::generic(binary->getLineNum(), "The operation '" + binary->getSym() + "' does not work with arrays.");
                return;
            }
        }
        if (isId(rhsExp))
        {
            Id *rhsId = (Id *)rhsExp;
            Decl *prevDecl = (Decl *)(getFromSymTable(rhsId->getName()));
            if ((prevDecl != nullptr && prevDecl->getData()->getIsArray()) || rhsId->getIsArray())
            {
                Emit::Error::generic(binary->getLineNum(), "The operation '" + binary->getSym() + "' does not work with arrays.");
                return;
            }
        }
    }
}

void Semantics::checkUnaryOperands(const Unary *unary) const
{
    if (!isUnary(unary))
    {
        throw std::runtime_error("Semantics::checkUnaryOperands() - Invalid Unary");
    }
    if (!lhsExists((Exp *)unary))
    {
        throw std::runtime_error("Semantics::checkUnaryOperands() - LHS operand must exist");
    }

    std::vector<Node *> children = unary->getChildren();
    Exp *lhsExp = (Exp *)(children[0]);
    Data *lhsData = setAndGetExpData(lhsExp);

    if (lhsData->getType() == Data::Type::None)
    {
        return;
    }

    switch (unary->getType())
    {
        case Unary::Type::Chsign:
        case Unary::Type::Question:
            if (lhsData->getIsArray())
            {
                Emit::Error::generic(unary->getLineNum(), "The operation '" + unary->getSym() + "' does not work with arrays.");
            }
            if (lhsData->getType() != Data::Type::Int)
            {
                Emit::Error::generic(unary->getLineNum(), "Unary '" + unary->getSym() + "' requires an operand of type int but was given type " + lhsData->stringify() + ".");
            }
            break;
        case Unary::Type::Sizeof:
            if (!lhsData->getIsArray())
            {
                Emit::Error::generic(unary->getLineNum(), "The operation 'sizeof' only works with arrays.");
            }
            break;
        case Unary::Type::Not:
            if (lhsData->getType() != Data::Type::Bool)
            {
                Emit::Error::generic(unary->getLineNum(), "Unary '" + unary->getSym() + "' requires an operand of type bool but was given type " + lhsData->stringify() + ".");
            }
            if (lhsData->getIsArray())
            {
                Emit::Error::generic(unary->getLineNum(), "The operation 'not' does not work with arrays.");
            }
            break;
        default:
            throw std::runtime_error("Semantics::checkUnaryOperands() - Unknown type");
            break;
    }
}

void Semantics::checkUnaryAsgnOperands(const UnaryAsgn *unaryAsgn) const
{
    if (!isUnaryAsgn(unaryAsgn))
    {
        throw std::runtime_error("Semantics::checkUnaryAsgnOperands() - Invalid UnaryAsgn");
    }
    if (!lhsExists((Exp *)unaryAsgn))
    {
        throw std::runtime_error("Semantics::checkUnaryAsgnOperands() - LHS operand must exist");
    }

    std::vector<Node *> children = unaryAsgn->getChildren();
    Exp *lhsExp = (Exp *)(children[0]);
    Data *lhsData = setAndGetExpData(lhsExp);
    if (lhsData->getType() == Data::Type::None)
    {
        return;
    }

    switch (unaryAsgn->getType())
    {
        case UnaryAsgn::Type::Inc:
        case UnaryAsgn::Type::Dec:
            if (lhsData->getIsArray())
            {
                Emit::Error::generic(unaryAsgn->getLineNum(), "The operation '" + unaryAsgn->getSym() + "' does not work with arrays.");
            }
            if (lhsData->getType() != Data::Type::Int)
            {
                Emit::Error::generic(unaryAsgn->getLineNum(), "Unary '" + unaryAsgn->getSym() + "' requires an operand of type int but was given type " + lhsData->stringify() + ".");
            }
            break;
        default:
            throw std::runtime_error("Semantics::checkUnaryAsgnOperands() - Unknown type");
            break;
    }
}

void Semantics::checkIndex(const Binary *binary) const
{
    if (binary->getType() != Binary::Type::Index)
    {
        throw std::runtime_error("Semantics::checkIndex() - Invalid Binary");
    }

    std::vector<Node *> children = binary->getChildren();
    Id *arrayId = (Id *)(children[0]);
    Node *indexNode = children[1];

    Decl *prevDecl = (Decl *)(getFromSymTable(arrayId->getName()));
    if (prevDecl == nullptr || !prevDecl->getData()->getIsArray() || !arrayId->getIsArray())
    {
        Emit::Error::generic(binary->getLineNum(), "Cannot index nonarray '" + arrayId->getName() + "'.");
    }

    if (prevDecl != nullptr && prevDecl->getData()->getIsArray() && isId(indexNode) )
    {
        Id *indexId = (Id *)indexNode;
        if (arrayId->getName().compare(indexId->getName()) == 0)
        {
            Emit::Error::generic(indexNode->getLineNum(), "Array index is the unindexed array '" + arrayId->getName() + "'.");
        }
    }

    Exp *indexExp = (Exp *)indexNode;
    Data *indexData = setAndGetExpData(indexExp);
    if (indexData->getType() != Data::Type::Int)
    {
        Emit::Error::generic(binary->getLineNum(), "Array '" + arrayId->getName() + "' should be indexed by type int but got type " + indexData->stringify() + ".");
    }
}

void Semantics::leaveScope()
{
    std::map<std::string, void *> syms = m_symTable->getSyms();
    for (auto const& [name, voisIdode] : syms)
    {
        Node *node = (Node *)voisIdode;
        if (!isDecl(node))
        {
            throw std::runtime_error("Semantics::leaveScope() - Illegal node found in symbol table");
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
        Decl *prevDecl = (Decl *)(getFromSymTable(decl->getName()));
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

bool Semantics::isMainFunc(const Func *func) const
{
    if (!isFunc(func))
    {
        throw std::runtime_error("Semantics::isMainFunc() - Invalid Func");
    }

    // Function name must be main and in global scope
    if (func->getName() != "main" || m_symTable->depth() != 1)
    {
        return false;
    }

    // Get the function children
    std::vector<Node *> funcChildren = func->getChildren();

    // There can't be any parms (children)
    if (funcChildren[0] != nullptr)
    {
        return false;
    }

    // If main is previously defined as a variable
    Decl *prevDecl = getFromSymTable(func->getName());
    if (isVar(prevDecl))
    {
        return false;
    }

    return true;
}

bool Semantics::isDeclared(const Id *id) const
{
    if (!isId(id))
    {
        throw std::runtime_error("Semantics::isDeclared() - Invalid Id");
    }

    // If the id name is not in the symbol table, it is not declared
    if (getFromSymTable(id->getName()) == nullptr)
    {
        return false;
    }
    return true;
}

bool Semantics::isIndex(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::isIndex() - Invalid Exp");
    }

    Node *lastParent = (Node *)exp;
    Node *parent = exp->getParent();
    while (parent != nullptr)
    {
        if (isBinary(parent))
        {
            Binary *binary = (Binary *)parent;
            if (binary->getType() == Binary::Type::Index)
            {
                // On the right side
                if (binary->getChildren()[1] == lastParent)
                {
                    return true;
                }
            }
        }
        lastParent = parent;
        parent = parent->getParent();
    }
    return false;
}

bool Semantics::expOperandsExist(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::expOperandsExist() - Invalid Exp");
    }

    std::vector<Node *> children = exp->getChildren();
    if (children.size() < 2 || children[0] == nullptr || children[1] == nullptr)
    {
        return false;
    }
    if (!isExp(children[0]) || !isExp(children[1]))
    {
        return false;
    }
    return true;
}

bool Semantics::lhsExists(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::lhsExists() - Invalid Exp");
    }

    std::vector<Node *> children = exp->getChildren();
    Exp *lhsExp = (Exp *)(children[0]);
    if (children.size() == 0 || lhsExp == nullptr)
    {
        return false;
    }
    return true;
}

std::string Semantics::getExpSym(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::getExpSym() - Invalid Exp");
    }

    if (isAsgn(exp))
    {
        Asgn *asgn = (Asgn *)exp;
        return asgn->getSym();
    }
    else if (isBinary(exp))
    {
        Binary *binary = (Binary *)exp;
        return  binary->getSym();
    }
    else
    {
        throw std::runtime_error("Semantics::getExpSym() - Exp is not an operation");
    }
}

Data * Semantics::setAndGetExpData(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::setAndGetExpData() - Invalid Exp");
    }

    std::string name;
    switch (exp->getExpKind())
    {
        case Exp::Kind::Asgn:
        {
            Asgn *asgn = (Asgn *)exp;
            Exp *lhsExp = (Exp *)(exp->getChildren()[0]);
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
            if (prevDecl != nullptr && prevDecl->getDeclKind() != Decl::Kind::Var)
            {
                call->setData(prevDecl->getData());
            }
            break;
        }
        case Exp::Kind::Id:
        {
            Id *id = (Id *)exp;
            Decl *prevDecl = getFromSymTable(id->getName());
            if (prevDecl != nullptr && prevDecl->getDeclKind() != Decl::Kind::Func)
            {
                id->setData(prevDecl->getData());
            }
            break;
        }
    }
    return exp->getData();
}
