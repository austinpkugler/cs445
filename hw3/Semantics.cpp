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
            if (stmt->getStmtKind() == Stmt::Kind::Range)
            {
                return;
            }
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

    // If the LHS is an id, it must have been declared
    std::vector<Node *> children = asgn->getChildren();
    if (isId(children[0]))
    {
        Id *lhsId = (Id *)(children[0]);
        // if (!isDeclaredId(lhsId))
        // {
        //     Emit::Error::generic(lhsId->getLineNum(), "Symbol \'" + lhsId->getName() + "\' is not declared.");
        // }
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
    // If the RHS is an id, it must have been declared
    // if (isId(children[1]))
    // {
    //     Id *rhsId = (Id *)(children[1]);
    //     if (!isDeclaredId(rhsId))
    //     {
    //         Emit::Error::generic(rhsId->getLineNum(), "Symbol \'" + rhsId->getName() + "\' is not declared.");
    //     }
    // }

    // LHS and RHS must be the same type
    if (asgn->getType() == Asgn::Type::Asgn)
    {
        checkOperandsAreSameType((Exp *)asgn);
    }
    else
    {
        checkAsgnOperands(asgn);
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
            checkBinaryIntOperands(binary);
            checkBinaryOperandsAreNotArray(binary);
            break;
        case Binary::Type::Index:
        {
            std::vector<Node *> children = binary->getChildren();
            checkArray((Id *)(children[0]), children[1]);
            break;
        }
        case Binary::Type::And:
        case Binary::Type::Or:
            checkBinaryBoolOperands(binary);
            break;
        case Binary::Type::LT:
        case Binary::Type::LEQ:
        case Binary::Type::GT:
        case Binary::Type::GEQ:
        case Binary::Type::EQ:
        case Binary::Type::NEQ:
            checkOperandsAreSameType((Exp *)binary);
            break;
        default:
            throw std::runtime_error("Semantics::analyzeBinary() - Unknown Binary");
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
        bool isIndexFlag = false;
        Node *parentNode = id->getParent();
        if (!id->getIsArray() && isBinary(parentNode))
        {
            Binary *parent = (Binary *)parentNode;
            if (parent->getType() == Binary::Type::Index)
            {
                isIndexFlag = true;
            }
        }

        if (!isIndexFlag && !prevDeclVar->getIsInitialized() && prevDeclVar->getShowErrors())
        {
            Emit::Warn::generic(id->getLineNum(), "Variable '" + id->getName() + "' may be uninitialized when used here.");
            prevDeclVar->setShowErrors(false);
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

    switch (unary->getType())
    {
        case Unary::Type::Chsign:
        case Unary::Type::Sizeof:
        case Unary::Type::Question:
        case Unary::Type::Not:
            checkUnaryOperands(unary);
            break;
        default:
            throw std::runtime_error("Semantics::analyzeUnary() - Unknown Unary");
            break;
    }
}

void Semantics::analyzeUnaryAsgn(const UnaryAsgn *unaryAsgn) const
{
    if (!isUnaryAsgn(unaryAsgn))
    {
        throw std::runtime_error("Semantics::analyzeUnaryAsgn() - Invalid UnaryAsgn");
    }

    checkUnaryAsgnOperands(unaryAsgn);
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
            break;
        case Stmt::Kind::Range:
            // Not analyzed
            throw std::runtime_error("Semantics::analyzeStmt() - Range Stmt");
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
            Decl *prevDecl = (Decl *)(getFromSymTable(id->getName()));
            if ((prevDecl != nullptr && prevDecl->getData()->getIsArray()) || id->getIsArray())
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

    // Get the function contents
    // Compound *compound = (Compound *)funcChildren[1];
    // std::vector<Node *> compoundChildren = compound->getChildren();

    // If the function is empty (main can't be empty in c-)
    // if (compoundChildren[0] == nullptr || compoundChildren[1] == nullptr)
    // {
    //     return false;
    // }

    return true;
}

bool Semantics::isDeclaredId(const Id *id) const
{
    if (!isId(id))
    {
        throw std::runtime_error("Semantics::isDeclaredId() - Invalid Id");
    }

    // If the id name is not in the symbol table, it is not declared
    if (getFromSymTable(id->getName()) == nullptr)
    {
        return false;
    }
    return true;
}

void Semantics::checkArray(const Id *arrayId, const Node *indexNode) const
{
    if (!isId(arrayId))
    {
        throw std::runtime_error("Semantics::checkArrayIndex() - Invalid Id");
    }

    Decl *prevDecl = (Decl *)(getFromSymTable(arrayId->getName()));
    if ((prevDecl != nullptr && !prevDecl->getData()->getIsArray()) || !arrayId->getIsArray())
    {
        Emit::Error::generic(arrayId->getLineNum(), "Cannot index nonarray '" + arrayId->getName() + "'.");
        
    }

    Exp *indexExp = (Exp *)indexNode;
    Data *indexData = setAndGetExpData(indexExp);
    if (indexData->getType() != Data::Type::Int)
    {
        Emit::Error::generic(indexNode->getLineNum(), "Array '" + arrayId->getName() + "' should be indexed by type int but got type " + indexData->stringify() + ".");
    }
}

void Semantics::checkOperandsAreSameType(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::checkOperandsAreSameType() - Invalid Exp");
    }

    if (!isBinary(exp) && !isAsgn(exp))
    {
        throw std::runtime_error("Semantics::checkOperandsAreSameType() - Exp is neither Binary nor Asgn");
    }

    if (!expOperandsExist(exp))
    {
        throw std::runtime_error("Semantics::checkOperandsAreSameType() - LHS and RHS Exp operands must exist");
    }

    std::vector<Node *> children = exp->getChildren();

    Exp *lhsExp = (Exp *)(children[0]);
    Exp *rhsExp = (Exp *)(children[1]);

    Data *lhsData = setAndGetExpData(lhsExp);
    Data *rhsData = setAndGetExpData(rhsExp);

    if (lhsData->getType() == Data::Type::None)
    {
        return;
    }

    if (lhsData->getType() != rhsData->getType())
    {
        if (isBinary(exp))
        {
            Binary *binary = (Binary *)exp;
            Emit::Error::generic(lhsExp->getLineNum(), "'" + binary->getSym() + "' requires operands of the same type but lhs is type " + lhsData->stringify() + " and rhs is type " + rhsData->stringify() + ".");
        }
        else if (isAsgn(exp))
        {
            Asgn *asgn = (Asgn *)exp;
            if (asgn->getType() == Asgn::Type::Asgn)
            {
                Emit::Error::generic(lhsExp->getLineNum(), "'=' requires operands of the same type but lhs is type " + lhsData->stringify() + " and rhs is type " + rhsData->stringify() + ".");
            }
            else
            {
                throw std::runtime_error("Semantics::checkOperandsAreSameType() - Exp is not a valid Asgn type");
            }
        }
    }
}

void Semantics::checkBinaryIntOperands(const Binary *binary) const
{
    if (!isBinary(binary))
    {
        throw std::runtime_error("Semantics::checkBinaryIntOperands() - Invalid Binary");
    }

    if (!expOperandsExist(binary))
    {
        throw std::runtime_error("Semantics::checkBinaryIntOperands() - LHS and RHS Exp operands must exist");
    }

    std::vector<Node *> children = binary->getChildren();
    Exp *lhsExp = (Exp *)(children[0]);
    Exp *rhsExp = (Exp *)(children[1]);
    Data *lhsData = setAndGetExpData(lhsExp);
    Data *rhsData = setAndGetExpData(rhsExp);

    if (lhsData->getType() == Data::Type::None || rhsData->getType() == Data::Type::None)
    {
        return;
    }

    if (lhsData->getType() != Data::Type::Int)
    {
        Emit::Error::generic(binary->getLineNum(), "'" + binary->getSym() + "' requires operands of type int but lhs is of type " + lhsData->stringify() + ".");
    }

    if (rhsData->getType() != Data::Type::Int)
    {
        Emit::Error::generic(binary->getLineNum(), "'" + binary->getSym() + "' requires operands of type int but rhs is of type " + lhsData->stringify() + ".");
    }
}

void Semantics::checkBinaryBoolOperands(const Binary *binary) const
{
    if (!isBinary(binary))
    {
        throw std::runtime_error("Semantics::checkBinaryBoolOperands() - Invalid Binary");
    }

    if (!expOperandsExist(binary))
    {
        throw std::runtime_error("Semantics::checkBinaryBoolOperands() - LHS and RHS Exp operands must exist");
    }

    std::vector<Node *> children = binary->getChildren();
    Exp *lhsExp = (Exp *)(children[0]);
    Exp *rhsExp = (Exp *)(children[1]);
    Data *lhsData = setAndGetExpData(lhsExp);
    Data *rhsData = setAndGetExpData(rhsExp);

    if (lhsData->getType() == Data::Type::None || rhsData->getType() == Data::Type::None)
    {
        return;
    }

    if (lhsData->getType() != Data::Type::Bool)
    {
        Emit::Error::generic(binary->getLineNum(), "'" + binary->getSym() + "' requires operands of type bool but lhs is of type " + lhsData->stringify() + ".");
    }

    if (rhsData->getType() != Data::Type::Bool)
    {
        Emit::Error::generic(binary->getLineNum(), "'" + binary->getSym() + "' requires operands of type bool but rhs is of type " + lhsData->stringify() + ".");
    }
}

void Semantics::checkBinaryOperandsAreNotArray(const Binary *binary) const
{
    if (!isBinary(binary))
    {
        throw std::runtime_error("Semantics::checkBinaryOperandsAreNotArray() - Invalid Binary");
    }

    if (!expOperandsExist(binary))
    {
        throw std::runtime_error("Semantics::checkBinaryOperandsAreNotArray() - LHS and RHS Exp operands must exist");
    }

    std::vector<Node *> children = binary->getChildren();

    Exp *lhsExp = (Exp *)(children[0]);
    Exp *rhsExp = (Exp *)(children[1]);

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

void Semantics::checkAsgnOperands(const Asgn *asgn) const
{
    if (!isAsgn(asgn) || asgn->getType() == Asgn::Type::Asgn)
    {
        throw std::runtime_error("Semantics::checkAsgnOperands() - Invalid Asgn");
    }

    if (!expOperandsExist(asgn))
    {
        throw std::runtime_error("Semantics::checkAsgnOperands() - LHS and RHS Exp operands must exist");
    }

    std::vector<Node *> children = asgn->getChildren();
    Exp *lhsExp = (Exp *)(children[0]);
    Exp *rhsExp = (Exp *)(children[1]);
    Data *lhsData = setAndGetExpData(lhsExp);
    Data *rhsData = setAndGetExpData(rhsExp);

    if (lhsData->getType() == Data::Type::None || rhsData->getType() == Data::Type::None)
    {
        return;
    }

    if (lhsData->getType() != Data::Type::Int)
    {
        Emit::Error::generic(asgn->getLineNum(), "'" + asgn->getSym() + "' requires operands of type int but lhs is of type " + rhsData->stringify() + ".");
    }

    if (rhsData->getType() != Data::Type::Int)
    {
        Emit::Error::generic(asgn->getLineNum(), "'" + asgn->getSym() + "' requires operands of type int but rhs is of type " + rhsData->stringify() + ".");
    }
}

void Semantics::checkUnaryOperands(const Unary *unary) const
{
    if (!isUnary(unary))
    {
        throw std::runtime_error("Semantics::checkUnaryOperands() - Invalid Unary");
    }

    std::vector<Node *> children = unary->getChildren();
    Exp *lhsExp = (Exp *)(children[0]);

    if (children.size() == 0 || lhsExp == nullptr)
    {
        throw std::runtime_error("Semantics::checkUnaryOperands() - LHS operand must exist");
    }

    Data *lhsData = setAndGetExpData(lhsExp);

    switch (unary->getType())
    {
        case Unary::Type::Chsign:
        case Unary::Type::Question:
            if (lhsData->getType() != Data::Type::Int)
            {
                Emit::Error::generic(unary->getLineNum(), "Unary '" + unary->getSym() + "' requires an operand of type int but was given type " + lhsData->stringify() + ".");
            }
            break;
        case Unary::Type::Sizeof:
            break;
        case Unary::Type::Not:
            if (lhsData->getType() != Data::Type::Bool)
            {
                Emit::Error::generic(unary->getLineNum(), "Unary '" + unary->getSym() + "' requires an operand of type bool but was given type " + lhsData->stringify() + ".");
            }
            break;
        default:
            throw std::runtime_error("Semantics::checkUnaryOperands() - Unknown Unary");
            break;
    }
}

void Semantics::checkUnaryAsgnOperands(const UnaryAsgn *unaryAsgn) const
{
    if (!isUnaryAsgn(unaryAsgn))
    {
        throw std::runtime_error("Semantics::checkUnaryAsgnOperands() - Invalid UnaryAsgn");
    }

    std::vector<Node *> children = unaryAsgn->getChildren();
    Exp *lhsExp = (Exp *)(children[0]);

    if (children.size() == 0 || lhsExp == nullptr)
    {
        throw std::runtime_error("Semantics::checkUnaryAsgnOperands() - LHS operand must exist");
    }

    Data *lhsData = setAndGetExpData(lhsExp);

    switch (unaryAsgn->getType())
    {
        case UnaryAsgn::Type::Inc:
        case UnaryAsgn::Type::Dec:
            if (lhsData->getType() != Data::Type::Int)
            {
                Emit::Error::generic(unaryAsgn->getLineNum(), "Unary '" + unaryAsgn->getSym() + "' requires an operand of type int but was given type " + lhsData->stringify() + ".");
            }
            break;
        default:
            throw std::runtime_error("Semantics::checkUnaryAsgnOperands() - Unknown UnaryAsgn");
            break;
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
