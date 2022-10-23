#include "Semantics.hpp"

Semantics::Semantics(SymTable *symTable) : m_symTable(symTable), m_mainExists(false) {}

void Semantics::analyze(Node *node)
{
    // Initialize the symbol table
    symTableSimpleEnterScope("Symbol table initialization");
    symTableInitialize(node);
    symTableSimpleLeaveScope();

    analyzeTree(node);
    checkUnusedWarns();

    if (!m_mainExists)
    {
        Emit::error("LINKER", "A function named 'main()' must be defined.");
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
        case Node::Kind::Func:
            analyzeFunc((Func *)node);
            break;
        case Node::Kind::Parm:
            analyzeParm((Parm *)node);
            break;
        case Node::Kind::Var:
            analyzeVar((Var *)node);
            break;
        case Node::Kind::Asgn:
            analyzeAsgn((Asgn *)node);
            break;
        case Node::Kind::Binary:
            analyzeBinary((Binary *)node);
            break;
        case Node::Kind::Call:
            analyzeCall((Call *)node);
            break;
        case Node::Kind::Const:
            // Not analyzed
            break;
        case Node::Kind::Id:
            analyzeId((Id *)node);
            break;
        case Node::Kind::Unary:
            analyzeUnary((Unary *)node);
            break;
        case Node::Kind::UnaryAsgn:
            analyzeUnaryAsgn((UnaryAsgn *)node);
            break;
        case Node::Kind::Break:
        case Node::Kind::Compound:
        case Node::Kind::For:
        case Node::Kind::If:
        case Node::Kind::Range:
            // Not analyzed
            break;
        case Node::Kind::Return:
            analyzeReturn((Return *)node);
            break;
        case Node::Kind::While:
            // Not analyzed
            break;
        default:
            throw std::runtime_error("Semantics::analyzeTree() - Invalid Node");
    }

    // Analyze children
    symTableEnterScope(node);
    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        analyzeTree(children[i]);
    }
    symTableLeaveScope(node);

    // Analyze sibling
    analyzeTree(node->getSibling());
}

void Semantics::analyzeFunc(const Func *func)
{
    if (!isFunc(func))
    {
        throw std::runtime_error("Semantics::analyzeFunc() - Invalid Func");
    }

    symTableInsert(func);

    if (isMainFunc(func))
    {
        m_mainExists = true;
    }
}

void Semantics::analyzeParm(const Parm *parm)
{
    if (!isParm(parm))
    {
        throw std::runtime_error("Semantics::analyzeParm() - Invalid Parm");
    }

    symTableInsert(parm);
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
        var->makeUsed();
    }
    if (var->getData()->getIsStatic())
    {
        var->makeInitialized();
    }
    // if (m_symTable->depth() == 1 || var->getData()->getIsStatic())
    // {
    //     var->makeInitialized();
    // }

    // Check for initializer errors if there is a child
    // if (isExp(var->getChild()))
    // {
    //     Exp *exp = (Exp *)(var->getChild());
    // }

    symTableInsert(var);
}

void Semantics::analyzeAsgn(const Asgn *asgn)
{
    if (!isAsgn(asgn))
    {
        throw std::runtime_error("Semantics::analyzeAsgn() - Invalid Asgn");
    }

    // If the LHS is an id, it must have been declared
    Node *lhs = asgn->getChild();
    Node *rhs = asgn->getChild(1);

    analyzeTree(rhs);

    if (isId(lhs))
    {
        Id *lhsId = (Id *)(lhs);
        Var *prevDeclLhsVar = (Var *)(symTableGet(lhsId->getName()));
        if (isVar(prevDeclLhsVar))
        {
            prevDeclLhsVar->makeInitialized();
        }
    }
    else
    {
        Binary *lhsBinary = (Binary *)(lhs);
        Id *arrayId = (Id *)(lhsBinary->getChild());
        Var *arrayDecl = (Var *)(symTableGet(arrayId->getName()));
        if (arrayDecl != nullptr)
        {
            arrayDecl->makeInitialized();
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
            throw std::runtime_error("Semantics::analyzeAsgn() - Invalid Asgn");
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
            throw std::runtime_error("Semantics::analyzeBinary() - Invalid Binary");
            break;
    }
}

void Semantics::analyzeCall(const Call *call) const
{
    if (!isCall(call))
    {
        throw std::runtime_error("Semantics::analyzeCall() - Invalid Call");
    }

    Decl *callDecl = (Decl *)(symTableGet(call->getName()));

    // If the function name is not in the symbol table
    if (callDecl == nullptr)
    {
        Emit::error(call->getLineNum(), "Symbol '" + call->getName() + "' is not declared.");
        return;
    }

    // If the function name is not associated with a function
    if (!isFunc(callDecl))
    {
        Emit::error(call->getLineNum(), "'" + call->getName() + "' is a simple variable and cannot be called.");
    }
    callDecl->makeUsed();
}

void Semantics::analyzeId(const Id *id) const
{
    if (!isId(id))
    {
        throw std::runtime_error("Semantics::analyzeId() - Invalid Id");
    }

    Decl *idDecl = (Decl *)(symTableGet(id->getName()));
    if (idDecl == nullptr)
    {
        Emit::error(id->getLineNum(), "Symbol '" + id->getName() + "' is not declared.");
        return;
    }

    if (isFunc(idDecl))
    {
        Emit::error(id->getLineNum(), "Cannot use function '" + id->getName() + "' as a variable.");
    }
    else if (isVar(idDecl))
    {
        Var *varDecl = (Var *)idDecl;

        // Don't warn if the uninitialized id is an array index (see hw4/test/lhs.c-)
        if (!varDecl->getIsInitialized() && varDecl->getShowErrors())
        {
            if (!hasIndexAncestor((Exp *)id) || idDecl->getData()->getIsArray())
            {
                if (!hasAsgnAncestor((Exp *)id))
                {
                    Emit::warn(id->getLineNum(), "Variable '" + id->getName() + "' may be uninitialized when used here.");
                    varDecl->setShowErrors(false);
                }
            }
        }
    }
    idDecl->makeUsed();
}

void Semantics::analyzeUnary(const Unary *unary) const
{
    if (!isUnary(unary))
    {
        throw std::runtime_error("Semantics::analyzeUnary() - Invalid Unary");
    }
    if (!lhsExists((Exp *)unary))
    {
        throw std::runtime_error("Semantics::analyzeUnary() - LHS operand must exist");
    }

    Exp *lhs = (Exp *)(unary->getChild());
    if (lhs->getData()->getType() == Data::Type::Undefined)
    {
        return;
    }

    switch (unary->getType())
    {
        case Unary::Type::Chsign:
        case Unary::Type::Question:
            if (lhs->getData()->getIsArray())
            {
                Emit::error(unary->getLineNum(), "The operation '" + unary->getSym() + "' does not work with arrays.");
            }
            if (lhs->getData()->getType() != Data::Type::Int)
            {
                Emit::error(unary->getLineNum(), "Unary '" + unary->getSym() + "' requires an operand of type int but was given type " + lhs->getData()->stringify() + ".");
            }
            break;
        case Unary::Type::Sizeof:
            if (!lhs->getData()->getIsArray())
            {
                Emit::error(unary->getLineNum(), "The operation 'sizeof' only works with arrays.");
            }
            break;
        case Unary::Type::Not:
            if (lhs->getData()->getType() != Data::Type::Bool)
            {
                Emit::error(unary->getLineNum(), "Unary '" + unary->getSym() + "' requires an operand of type bool but was given type " + lhs->getData()->stringify() + ".");
            }
            if (lhs->getData()->getIsArray())
            {
                Emit::error(unary->getLineNum(), "The operation 'not' does not work with arrays.");
            }
            break;
        default:
            throw std::runtime_error("Semantics::analyzeUnary() - Invalid Unary");
            break;
    }
}

void Semantics::analyzeUnaryAsgn(const UnaryAsgn *unaryAsgn) const
{
    if (!isUnaryAsgn(unaryAsgn))
    {
        throw std::runtime_error("Semantics::analyzeUnaryAsgn() - Invalid UnaryAsgn");
    }
    if (!lhsExists((Exp *)unaryAsgn))
    {
        throw std::runtime_error("Semantics::analyzeUnaryAsgn() - LHS operand must exist");
    }

    Exp *lhs = (Exp *)(unaryAsgn->getChild());
    if (lhs->getData()->getType() == Data::Type::Undefined)
    {
        return;
    }

    switch (unaryAsgn->getType())
    {
        case UnaryAsgn::Type::Inc:
        case UnaryAsgn::Type::Dec:
            if (lhs->getData()->getIsArray())
            {
                Emit::error(unaryAsgn->getLineNum(), "The operation '" + unaryAsgn->getSym() + "' does not work with arrays.");
            }
            if (lhs->getData()->getType() != Data::Type::Int)
            {
                Emit::error(unaryAsgn->getLineNum(), "Unary '" + unaryAsgn->getSym() + "' requires an operand of type int but was given type " + lhs->getData()->stringify() + ".");
            }
            break;
        default:
            throw std::runtime_error("Semantics::analyzeUnaryAsgn() - Invalid UnaryAsgn");
            break;
    }
}

void Semantics::analyzeReturn(const Return *returnN) const
{
    if (!isReturn(returnN))
    {
        throw std::runtime_error("Semantics::analyzeReturn() - Invalid Return");
    }

    if (returnN->getChildCount() > 0)
    {
        Exp *lhs = (Exp *)(returnN->getChild());
        if (isId(lhs))
        {
            Id *lhsId = (Id *)lhs;
            Decl *idDecl = (Decl *)(symTableGet(lhsId->getName()));
            if ((idDecl != nullptr && idDecl->getData()->getIsArray()))
            {
                Emit::error(returnN->getLineNum(), "Cannot return an array.");
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
    Exp *lhs = (Exp *)(exp->getChild());
    Exp *rhs = (Exp *)(exp->getChild(1));

    // Ignore cases where the LHS has no type
    if (lhs->getData()->getType() == Data::Type::Undefined)
    {
        return;
    }

    // Both sides must be the same type
    if (lhs->getData()->getType() != rhs->getData()->getType())
    {
        Emit::error(exp->getLineNum(), "'" + sym + "' requires operands of the same type but lhs is type " + lhs->getData()->stringify() + " and rhs is type " + rhs->getData()->stringify() + ".");
    }

    // Both sides must be arrays or both must not be arrays
    if (lhs->getData()->getIsArray() && !rhs->getData()->getIsArray())
    {
        Emit::error(exp->getLineNum(), "'" + sym + "' requires both operands be arrays or not but lhs is an array and rhs is not an array.");
    }
    else if (!lhs->getData()->getIsArray() && rhs->getData()->getIsArray())
    {
        Emit::error(exp->getLineNum(), "'" + sym + "' requires both operands be arrays or not but lhs is not an array and rhs is an array.");
    }

    if (isId(lhs) && isId(rhs))
    {
        Id *lhsId = (Id *)lhs;
        Id *rhsId = (Id *)rhs;
        if (lhsId->getName() != rhsId->getName())
        {
            Decl *prevLhsDecl = symTableGet(lhsId->getName());
            Decl *prevRhsDecl = symTableGet(rhsId->getName());
            if ((prevLhsDecl != nullptr && isVar(prevLhsDecl)) && (prevRhsDecl != nullptr && isVar(prevRhsDecl)))
            {
                if (rhs->getData()->getCopyOf() != lhsId->getName())
                {
                    rhs->getData()->setCopyOf(rhsId->getName());
                }
            }
        }
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
    std::string typeString = Data::typeToString(type);
    Exp *lhs = (Exp *)(exp->getChild());
    Exp *rhs = (Exp *)(exp->getChild(1));

    // Ignore cases where the LHS or RHS has no type
    if (lhs->getData()->getType() == Data::Type::Undefined || rhs->getData()->getType() == Data::Type::Undefined)
    {
        return;
    }

    if (lhs->getData()->getType() != type)
    {
        Emit::error(exp->getLineNum(), "'" + sym + "' requires operands of type " + typeString + " but lhs is of type " + lhs->getData()->stringify() + ".");
    }

    if (rhs->getData()->getType() != type)
    {
        Emit::error(exp->getLineNum(), "'" + sym + "' requires operands of type " + typeString + " but rhs is of type " + rhs->getData()->stringify() + ".");
    }

    // If it is a binary operation, we want the operands to be only the passed type (not an array of that type)
    if (isBinary(exp))
    {
        Binary *binary = (Binary *)exp;
        if (isId(lhs))
        {
            Id *lhsId = (Id *)lhs;
            Decl *prevDecl = symTableGet(lhsId->getName());
            if ((prevDecl != nullptr && prevDecl->getData()->getIsArray()))
            {
                Emit::error(binary->getLineNum(), "The operation '" + binary->getSym() + "' does not work with arrays.");
                return;
            }
        }
        if (isId(rhs))
        {
            Id *rhsId = (Id *)rhs;
            Decl *prevDecl = symTableGet(rhsId->getName());
            if ((prevDecl != nullptr && prevDecl->getData()->getIsArray()))
            {
                Emit::error(binary->getLineNum(), "The operation '" + binary->getSym() + "' does not work with arrays.");
                return;
            }
        }
    }
}

void Semantics::checkIndex(const Binary *binary) const
{
    if (binary->getType() != Binary::Type::Index)
    {
        throw std::runtime_error("Semantics::checkIndex() - Invalid Binary");
    }

    Id *arrayId = (Id *)(binary->getChild());
    Exp *indexExp = (Exp *)(binary->getChild(1));

    Decl *arrayDecl = (Decl *)(symTableGet(arrayId->getName()));
    if (arrayDecl == nullptr || !arrayDecl->getData()->getIsArray())
    {
        Emit::error(binary->getLineNum(), "Cannot index nonarray '" + arrayId->getName() + "'.");
    }

    if (indexExp->getData()->getType() != Data::Type::Int)
    {
        Emit::error(binary->getLineNum(), "Array '" + arrayId->getName() + "' should be indexed by type int but got type " + indexExp->getData()->stringify() + ".");
    }

    if (isId(indexExp))
    {
        Id *indexId = (Id *)indexExp;
        if (arrayDecl != nullptr && arrayDecl->getData()->getIsArray())
        {
            if (indexId->getName() == arrayId->getName())
            {
                Emit::error(binary->getLineNum(), "Array index is the unindexed array '" + arrayId->getName() + "'.");
            }
        }
        else if (arrayDecl != nullptr)
        {
            Var *arrayVar = (Var *)arrayDecl;
            if (indexId->getName() == arrayVar->getData()->getCopyOf())
            {
                Emit::error(binary->getLineNum(), "Array index is the unindexed array '" + indexId->getName() + "'.");
            }
        }
    }
}

void Semantics::checkUnusedWarns() const
{
    std::map<std::string, void *> syms = m_symTable->getSyms();
    for (auto const& [name, currNode] : syms)
    {
        Decl *decl = (Decl *)currNode;
        if (isVar(decl) || isParm(decl))
        {
            if (!decl->getIsUsed())
            {
                Emit::warn(decl->getLineNum(), "The variable '" + decl->getName() + "' seems not to be used.");
            }
        }
    }
}

bool Semantics::symTableInsert(const Decl *decl, const bool global, const bool showWarns)
{
    if (!isDecl(decl))
    {
        throw std::runtime_error("Semantics::symTableInsert() - Invalid Decl");
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

    if (!inserted && showWarns)
    {
        Decl *prevDecl = (Decl *)(symTableGet(decl->getName()));
        if (prevDecl == nullptr)
        {
            throw std::runtime_error("Semantics::symTableInsert() - Failed to insert Decl");
        }
        std::stringstream msg;
        msg << "Symbol '" << decl->getName() << "' is already declared at line " << prevDecl->getLineNum() << ".";
        Emit::error(decl->getLineNum(), msg.str());
    }

    return inserted;
}

Decl * Semantics::symTableGet(const std::string name) const
{
    if (name.length() == 0)
    {
        throw std::runtime_error("Semantics::symTableGet() - Invalid name");
    }

    Decl *prevDecl = (Decl *)(m_symTable->lookup(name));
    return prevDecl;
}

void Semantics::symTableInitialize(Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    if (isDecl(node))
    {
        symTableInsert((Decl *)node, false, false);
    }

    symTableSetType(node);
    symTableEnterScope(node);

    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        symTableInitialize(children[i]);
    }

    symTableLeaveScope(node, false);
    symTableInitialize(node->getSibling());
}

Data * Semantics::symTableSetType(Node* node)
{
    if (node == nullptr || !isExp2(node))
    {
        return new Data(Data::Type::Undefined, false, false);
    }

    auto expNode = (Exp *)node;

    int lineNum = expNode->getLineNum();
    std::string name;

    if (expNode->getNodeKind() == Node::Kind::Id)
    {
        name = ((Id *)expNode)->getName();
        Decl *declNode = (Decl *)(m_symTable->lookup(name));
        if (declNode && (declNode->getNodeKind() == Node::Kind::Var || declNode->getNodeKind() == Node::Kind::Parm))
        {
            expNode->setData(declNode->getData());
        }
    }
    else if (expNode->getNodeKind() == Node::Kind::Call)
    {
        name = ((Call *)expNode)->getName();
        Decl *declNode = (Decl *)(m_symTable->lookup(name));
        if (declNode && declNode->getNodeKind() == Node::Kind::Func)
        {
            expNode->setData(declNode->getData());
        }
    
    }
    else if (expNode->getNodeKind() == Node::Kind::Binary)
    {
        auto binaryOpNode = (Binary *)(expNode);
        auto lval = (Exp *)(expNode->getChild(0));
        auto rval = (Exp *)(expNode->getChild(1));

        if (binaryOpNode->getType() == Binary::Type::Index)
        {
            expNode->setData(symTableSetType(lval)->getNextData());
        }
        else if (symTableSetType(lval)->getType() == Data::Type::Undefined || 
                   symTableSetType(rval)->getType() == Data::Type::Undefined)
        {
            expNode->setData(new Data(Data::Type::Undefined, false, false));
        }
    }
    else if (expNode->getNodeKind() == Node::Kind::Asgn)
    {
        Asgn *asgn = (Asgn *)expNode;
        auto lval = (Exp *)(expNode->getChild(0));
        auto rval = (Exp *)(expNode->getChild(1));

        if (asgn->getType() == Asgn::Type::Asgn)
        {
            expNode->setData(symTableSetType(lval)->getNextData());
        }
        else if (symTableSetType(lval)->getType() == Data::Type::Undefined || 
                   symTableSetType(rval)->getType() == Data::Type::Undefined)
        {
            expNode->setData(new Data(Data::Type::Undefined, false, false));
        }
    }
    else if (expNode->getNodeKind() == Node::Kind::Unary)
    {
        auto rval = (Exp *)(expNode->getChild(0));
        if (symTableSetType(rval)->getType() == Data::Type::Undefined)
        {
            expNode->setData(new Data(Data::Type::Undefined, false, false));
        }
    }

    return expNode->getData();
}

// Data * Semantics::symTableSetType(Node *node)
// {
//     if (!isExp(node))
//     {
//         return new Data(Data::Type::Undefined, false, false);
//     }

//     Exp *exp = (Exp *)node;
//     switch (exp->getNodeKind())
//     {
//         case Node::Kind::Asgn:
//         {
//             Asgn *asgn = (Asgn *)exp;
//             if (asgn->getType() == Asgn::Type::Asgn)
//             {
//                 exp->setData(symTableSetType(asgn->getChild())->getNextData());
//             }
//             else if (symTableSetType(asgn->getChild())->getType() == Data::Type::Undefined || symTableSetType(asgn->getChild(1))->getType() == Data::Type::Undefined)
//             {
//                 exp->setData(new Data(Data::Type::Undefined, false, false));
//             }
//         }
//         case Node::Kind::Binary:
//         {
//             Binary *binary = (Binary *)exp;
//             if (binary->getType() == Binary::Type::Index)
//             {
//                 exp->setData(symTableSetType(binary->getChild())->getNextData());
//             }
//             else if (symTableSetType(binary->getChild())->getType() == Data::Type::Undefined || symTableSetType(binary->getChild(1))->getType() == Data::Type::Undefined)
//             {
//                 exp->setData(new Data(Data::Type::Undefined, false, false));
//             }
//             break;
//         }
//         case Node::Kind::Call:
//         {
//             Decl *decl = symTableGet(((Call *)node)->getName());
//             if (isFunc(decl))
//             {
//                 exp->setData(decl->getData());
//             }
//             break;
//         }
//         case Node::Kind::Id:
//         {
//             Decl *decl = symTableGet(((Id *)node)->getName());
//             if (isVar(decl))
//             {
//                 exp->setData(decl->getData());
//             }
//             break;
//         }
//         case Node::Kind::Unary:
//         {
//             Unary *unary = (Unary *)exp;
//             if (symTableSetType(unary->getChild())->getType() == Data::Type::Undefined)
//             {
//                 exp->setData(new Data(Data::Type::Undefined, false, false));
//             }
//             break;
//         }
//     }
//     return exp->getData();
// }

void Semantics::symTableSimpleEnterScope(const std::string name)
{
    if (name.length() == 0)
    {
        throw std::runtime_error("Semantics::symTableEnterScope() - Invalid name");
    }

    m_symTable->enter(name);
}

void Semantics::symTableSimpleLeaveScope(const bool showWarns)
{
    if (showWarns)
    {
        checkUnusedWarns();
    }
    m_symTable->leave();
}

void Semantics::symTableEnterScope(const Node *node)
{
    if (node->getNodeKind() == Node::Kind::Compound)
    {
        if (node->getParent() != nullptr)
        {
            Node::Kind parentKind = node->getParent()->getNodeKind();
            if (parentKind == Node::Kind::For || parentKind == Node::Kind::Func)
            {
                return;
            }
        }
        symTableSimpleEnterScope("Compound statement");
        return;
    }

    switch (node->getNodeKind())
    {
        case Node::Kind::For:
            symTableSimpleEnterScope("For loop");
            break;
        case Node::Kind::Func:
            symTableSimpleEnterScope("Function body");
            break;
    }
}

void Semantics::symTableLeaveScope(const Node *node, const bool showWarns)
{
    Node::Kind nodeKind = node->getNodeKind();
    if (nodeKind == Node::Kind::For || nodeKind == Node::Kind::Func)
    {
        symTableSimpleLeaveScope(showWarns);
        return;
    }

    if (node->getParent() != nullptr)
    {
        Node::Kind parentKind = node->getParent()->getNodeKind();
        if (nodeKind == Node::Kind::Compound && (parentKind != Node::Kind::For && parentKind != Node::Kind::Func))
        {
            symTableSimpleLeaveScope(showWarns);
        }
    }
    else
    {
        if (nodeKind == Node::Kind::Compound)
        {
            symTableSimpleLeaveScope(showWarns);
        }
    }
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

    // There can't be any parms (children)
    if (func->getChild() != nullptr)
    {
        return false;
    }

    // If main is previously defined as a variable
    Decl *decl = symTableGet(func->getName());
    if (isVar(decl))
    {
        return false;
    }
    return true;
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
        return binary->getSym();
    }
    else
    {
        throw std::runtime_error("Semantics::getExpSym() - Exp is not an operation");
    }
}

bool Semantics::hasIndexAncestor(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::hasIndexAncestor() - Invalid Exp");
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
                if (binary->getChild(1) == lastParent)
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

bool Semantics::hasAsgnAncestor(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::hasAsgnAncestor() - Invalid Exp");
    }

    Node *lastParent = (Node *)exp;
    Node *parent = exp->getParent();
    while (parent != nullptr)
    {
        if (isAsgn(parent))
        {
            Asgn *asgn = (Asgn *)parent;
            if (asgn->getType() == Asgn::Type::Asgn)
            {
                // On the left side
                if (asgn->getChild() == lastParent)
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
