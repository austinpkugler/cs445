#include "Semantics.hpp"

Semantics::Semantics(SymTable *symTable) : m_symTable(symTable), m_mainExists(false), m_ioRoot(nullptr) {}

void Semantics::analyze(Node *node)
{
    symTableInitializeIOTree();
    symTableInjectIOTree(m_ioRoot);

    // Initialize the symbol table
    symTableSimpleEnterScope("Symbol table initialization");
    symTableInitialize(node);
    symTableSimpleLeaveScope();

    analyzeTree(node);
    checkUnusedWarns();

    if (node && !m_mainExists)
    {
        Emit::error("LINKER", "A function named 'main' with no parameters must be defined.");
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
            analyzeConst((Const *)node);
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
            analyzeBreak((Break *)node);
            break;
        case Node::Kind::Compound:
            analyzeCompound((Compound *)node);
            break;
        case Node::Kind::For:
            analyzeFor((For *)node);
            break;
        case Node::Kind::If:
            analyzeIf((If *)node);
            break;
        case Node::Kind::Range:
            analyzeRange((Range *)node);
            break;
        case Node::Kind::Return:
            analyzeReturn((Return *)node);
            break;
        case Node::Kind::While:
            analyzeWhile((While *)node);
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

void Semantics::analyzeFunc(Func *func)
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

void Semantics::analyzeParm(Parm *parm)
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
    if (m_symTable->depth() == 1 || var->getData()->getIsStatic())
    {
        var->makeInitialized();
    }

    // Check for initializer errors if there is a child
    if (var->getChildCount() != 0 && isExp(var->getChild()))
    {
        Exp *exp = (Exp *)(var->getChild());
        analyzeTree(exp);

        Data *expData = exp->getData();
        Data *varData = var->getData();
        Data::Type expType = expData->getType();
        Data::Type varType = varData->getType();
        if (varType != Data::Type::Undefined && expType != Data::Type::Undefined)
        {
            if (varType != expType)
            {
                Emit::error(var->getLineNum(), "Initializer for variable '" + var->getName() + "' of type " + Data::typeToString(varType) + " is of type " + Data::typeToString(expType));
            }
        }

        if (varData->getIsArray() != expData->getIsArray())
        {
            if (varData->getIsArray())
            {
                Emit::error(var->getLineNum(), "Initializer for variable '" + var->getName() + "' requires both operands be arrays or not but variable is an array and rhs is not an array.");
            }
            else if (expData->getIsArray())
            {
                Emit::error(var->getLineNum(), "Initializer for variable '" + var->getName() + "' requires both operands be arrays or not but variable is not an array and rhs is an array.");
            }
        }

        if (hasNonConstantRelative(exp)) // if not a constant exp
        {
            Emit::error(var->getLineNum(), "Initializer for variable '" + var->getName() + "' is not a constant expression.");
        }
    }

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
        return;
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
            checkOperandsOfType((Exp *)binary, Data::Type::Bool, false);
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

    Decl *decl = (Decl *)(symTableGet(call->getName()));

    // If the function name is not in the symbol table
    if (decl == nullptr)
    {
        Emit::error(call->getLineNum(), "Symbol '" + call->getName() + "' is not declared.");
        return;
    }

    // If the function name is not associated with a function
    if (!isFunc(decl))
    {
        Emit::error(call->getLineNum(), "'" + call->getName() + "' is a simple variable and cannot be called.");
    }
    else
    {
        Func *func = (Func *)decl;
        unsigned funcParmCount = func->getParmCount();
        unsigned callParmCount = call->getParmCount();
        if (callParmCount < funcParmCount)
        {
            std::stringstream msg;
            msg << "Too few parameters passed for function '" << func->getName() << "' declared on line " << func->getLineNum() << ".";
            Emit::error(call->getLineNum(), msg.str());
        }
        else if (callParmCount > funcParmCount)
        {
            std::stringstream msg;
            msg << "Too many parameters passed for function '" << func->getName() << "' declared on line " << func->getLineNum() << ".";
            Emit::error(call->getLineNum(), msg.str());
        }

        Exp *callParm = (Exp *)(call->getChild());
        Var *funcParm = (Var *)(func->getChild());
        unsigned parmCount = 1;
        while (callParm != nullptr && funcParm != nullptr)
        {
            Data *callParmData = callParm->getData();
            Data *funcParmData = funcParm->getData();
            Data::Type callParmType = callParmData->getType();
            Data::Type funcParmType = funcParmData->getType();
            if (callParmType != Data::Type::Undefined && funcParmType != Data::Type::Undefined && callParmType != funcParmType)
            {
                std::stringstream msg;
                msg << "Expecting type " << Data::typeToString(funcParmType) << " in parameter " << parmCount << " of call to '" << func->getName() << "' declared on line " << func->getLineNum() <<" but got type " << Data::typeToString(callParmType) << ".";
                Emit::error(call->getLineNum(), msg.str());
            }

            if (callParmData->getIsArray() != funcParmData->getIsArray())
            {
                if (callParmData->getIsArray())
                {
                    std::stringstream msg;
                    msg << "Not expecting array in parameter " << parmCount << " of call to '" << call->getName() << "' declared on line " << func->getLineNum() << ".";
                    Emit::error(call->getLineNum(), msg.str());
                }
                else if (funcParmData->getIsArray())
                {
                    std::stringstream msg;
                    msg << "Expecting array in parameter " << parmCount << " of call to '" << call->getName() << "' declared on line " << func->getLineNum() << ".";
                    Emit::error(call->getLineNum(), msg.str());
                }
            }

            parmCount++;
            callParm = (Exp *)(callParm->getSibling());
            funcParm = (Var *)(funcParm->getSibling());
        }
    }

    decl->makeUsed();
}

void Semantics::analyzeConst(Const *constN) const
{
    if (!isConst(constN))
    {
        throw std::runtime_error("Semantics::analyzeConst() - Invalid Const");
    }
}

void Semantics::analyzeId(Id *id) const
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
            if (!hasAsgnRelative((Exp *)id))
            {
                Emit::warn(id->getLineNum(), "Variable '" + id->getName() + "' may be uninitialized when used here.");
                varDecl->setShowErrors(false);
            }
        }
    }

    id->setMemScope(idDecl->getMemScope());
    id->setMemSize(idDecl->getMemSize());
    id->setMemLoc(idDecl->getMemLoc());
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
        return;
        // throw std::runtime_error("Semantics::analyzeUnary() - LHS operand must exist");
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

void Semantics::analyzeBreak(const Break *breakN) const
{
    if (!isBreak(breakN))
    {
        throw std::runtime_error("Semantics::analyzeBreak() - Invalid Break");
    }

    bool inForLoop = breakN->hasRelative(Node::Kind::For);
    bool inWhileLoop = breakN->hasRelative(Node::Kind::While);
    if (!inForLoop && !inWhileLoop)
    {
        Emit::error(breakN->getLineNum(), "Cannot have a break statement outside of loop.");
    }
}

void Semantics::analyzeCompound(Compound *compound) const
{
    if (!isCompound(compound))
    {
        throw std::runtime_error("Semantics::analyzeCompound() - Invalid Compound");
    }

    int parmCount;
    Node *node = compound->getRelative(Node::Kind::Func);
    if (isFunc(node))
    {
        Func *func = (Func *)node;
        parmCount = func->getParmCount();
    }
}

void Semantics::analyzeFor(For *forN) const
{
    if (!isFor(forN))
    {
        throw std::runtime_error("Semantics::analyzeFor() - Invalid For");
    }
}

void Semantics::analyzeIf(const If *ifN) const
{
    if (!isIf(ifN))
    {
        throw std::runtime_error("Semantics::analyzeIf() - Invalid If");
    }

    Exp *lhs = (Exp *)(ifN->getChild());
    if (lhs->getData()->getIsArray())
    {
        Emit::error(ifN->getLineNum(), "Cannot use array as test condition in if statement.");
    }

    if (lhs->getData()->getType() != Data::Type::Bool && lhs->getData()->getType() != Data::Type::Undefined)
    {
        Emit::error(ifN->getLineNum(), "Expecting Boolean test condition in if statement but got type " + lhs->getData()->stringify() + ".");
    }
}

void Semantics::analyzeRange(const Range *range) const
{
    if (!isRange(range))
    {
        throw std::runtime_error("Semantics::analyzeRange() - Invalid Range");
    }

    std::vector<Node *> children = range->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        if (children[i] == nullptr)
        {
            continue;
        }

        bool isArray = false;
        if (isId(children[i]))
        {
            Id *id = (Id *)(children[i]);
            if (id->getData()->getIsArray())
            {
                isArray = true;
            }
        }
        if (isArray)
        {
            std::stringstream msg;
            msg << "Cannot use array in position " << i + 1 << " in range of for statement.";
            Emit::error(children[i]->getLineNum(), msg.str());
        }

        Exp *exp = (Exp *)(children[i]);
        if (isExp(exp) && exp->getData()->getType() != Data::Type::Undefined && exp->getData()->getType() != Data::Type::Int)
        {
            std::stringstream msg;
            msg << "Expecting type int in position " << i + 1 << " in range of for statement but got type " << exp->getData()->stringify() << ".";
            Emit::error(range->getLineNum(), msg.str());
        }
    }
}

void Semantics::analyzeReturn(const Return *returnN) const
{
    if (!isReturn(returnN))
    {
        throw std::runtime_error("Semantics::analyzeReturn() - Invalid Return");
    }

    Func *func = (Func *)(returnN->getRelative(Node::Kind::Func));
    func->makeHasReturn();
    if (returnN->getChildCount() == 0)
    {
        if (func->getData()->getType() != Data::Type::Void)
        {
            std::stringstream msg;
            msg << "Function '" << func->getName() << "' at line " << func->getLineNum() << " is expecting to return type " << func->getData()->stringify() << " but return has no value.";
            Emit::error(returnN->getLineNum(), msg.str());
        }
        return;
    }

    Exp *returnExp = (Exp *)(returnN->getChild());
    if (returnExp->getData()->getIsArray())
    {
        Emit::error(returnN->getLineNum(), "Cannot return an array.");
    }

    if (func->getData()->getType() != returnExp->getData()->getType())
    {
        if (func->getData()->getType() == Data::Type::Void)
        {
            std::stringstream msg;
            msg << "Function '" << func->getName() << "' at line " << func->getLineNum() << " is expecting no return value, but return has a value.";
            Emit::error(returnN->getLineNum(), msg.str());
        }
        else if (returnExp->getData()->getType() != Data::Type::Undefined)
        {
            std::stringstream msg;
            msg << "Function '" << func->getName() << "' at line " << func->getLineNum() << " is expecting to return type " << func->getData()->stringify() << " but returns type " << returnExp->getData()->stringify() << ".";
            Emit::error(returnN->getLineNum(), msg.str());
        }
    }
}

void Semantics::analyzeWhile(const While *whileN) const
{
    if (!isWhile(whileN))
    {
        throw std::runtime_error("Semantics::analyzeWhile() - Invalid While");
    }

    Exp *testExp = (Exp *)(whileN->getChild());
    if (!isExp(testExp))
    {
        return;
    }

    Data *testData = testExp->getData();
    if (testData->getType() != Data::Type::Undefined)
    {
        if (testData->getType() != Data::Type::Bool)
        {
            Emit::error(whileN->getLineNum(), "Expecting Boolean test condition in while statement but got type " + testExp->getData()->stringify() + ".");
        }

        if (testData->getIsArray())
        {
            Emit::error(whileN->getLineNum(), "Cannot use array as test condition in while statement.");
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
    if (lhs->getData()->getType() == Data::Type::Undefined || rhs->getData()->getType() == Data::Type::Undefined)
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
}

void Semantics::checkOperandsOfType(Exp *exp, const Data::Type type, const bool isMath) const
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

    // Ignore cases where the LHS is undefined
    if (lhs->getData()->getType() != type && lhs->getData()->getType() != Data::Type::Undefined)
    {
        Emit::error(exp->getLineNum(), "'" + sym + "' requires operands of type " + typeString + " but lhs is of type " + lhs->getData()->stringify() + ".");
    }
    // Ignore cases where the RHS is undefined
    if (rhs->getData()->getType() != type && rhs->getData()->getType() != Data::Type::Undefined)
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

    if (indexExp->getData()->getType() != Data::Type::Undefined && indexExp->getData()->getType() != Data::Type::Int)
    {
        Emit::error(binary->getLineNum(), "Array '" + arrayId->getName() + "' should be indexed by type int but got type " + indexExp->getData()->stringify() + ".");
    }

    if (isId(indexExp))
    {
        Id *indexId = (Id *)indexExp;
        Decl *indexDecl = (Decl *)(symTableGet(indexId->getName()));
        if (isDecl(indexDecl) && indexDecl->getData()->getIsArray())
        {
            Emit::error(binary->getLineNum(), "Array index is the unindexed array '" + indexId->getName() + "'.");
        }
    }
}

void Semantics::checkUnusedWarns() const
{
    std::map<std::string, void *> syms = m_symTable->getSyms();
    for (auto const& [name, currNode] : syms)
    {
        Decl *decl = (Decl *)currNode;
        if (isVar(decl) && !decl->getIsUsed())
        {
            Emit::warn(decl->getLineNum(), "The variable '" + decl->getName() + "' seems not to be used.");
        }
        else if (isParm(decl) && !decl->getIsUsed())
        {
            Emit::warn(decl->getLineNum(), "The parameter '" + decl->getName() + "' seems not to be used.");
        }
        else if (isFunc(decl) && !decl->getIsUsed() && decl->getName() != "main")
        {
            Emit::warn(decl->getLineNum(), "The function '" + decl->getName() + "' seems not to be used.");
        }

        // if (isFunc(decl))
        // {
        //     Func *func = (Func *)decl;
        //     // Emit::warn(func->getLineNum(), "Checking func: " + func->getName());
        //     // Emit::warn(func->getLineNum(), "Has return: " + func->getHasReturn());
        //     if (func->getData()->getType() != Data::Type::Undefined && func->getData()->getType() != Data::Type::Void && !func->getHasReturn())
        //     {
        //         Emit::warn(func->getLineNum(), "Expecting to return type " + func->getData()->stringify() + " but function '" + func->getName() + "' has no return statement.");
        //     }
        // }
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
        if (isFunc(node))
        {
            s_foffsets.push_back(-2);
        }
    }

    symTableSetType(node);

    if (m_symTable->depth() == 2)
    {
        node->setMemScope("Global");
    }
    else
    {
        if (isVar(node))
        {
            Var *var = (Var *)node;
            if (var->getData()->getIsStatic())
            {
                var->setMemScope("LocalStatic");
            }
            else
            {
                var->setMemScope("Local");
            }
        }
        else if (isParm(node))
        {
            node->setMemScope("Parameter");
        }
    }

    bool hasScope = symTableEnterScope(node);
    if (hasScope)
    {
        if (isFunc(node))
        {
            s_foffsets.push_back(-2);
        }
        else
        {
            s_foffsets.push_back(s_foffsets.back());
        }
    }

    if (node->getMemScope() == "Global" || node->getMemScope() == "LocalStatic")
    {
        if (isConst(node))
        {
            Const *constN = (Const *)node;
            if (constN->getType() == Const::Type::String)
            {
                constN->setMemLoc(s_goffset - 1);
            }
            else
            {
                constN->setMemLoc(s_goffset);
            }
            s_goffset -= node->getMemSize();
        }
        else if (isVar(node))
        {
            Var *var = (Var *)node;
            if (var->getData()->getIsArray())
            {
                var->setMemLoc(s_goffset - 1);
            }
            else
            {
                var->setMemLoc(s_goffset);
            }
            s_goffset -= node->getMemSize();
        }
    }
    else if (node->getMemScope() == "Local")
    {
        if (isVar(node))
        {
            Var *var = (Var *)node;
            if (var->getData()->getIsArray())
            {
                var->setMemLoc(s_foffsets.back() - 1);
            }
            else
            {
                var->setMemLoc(s_foffsets.back());
            }
            s_foffsets.back() -= node->getMemSize();
        }
    }
    else if (node->getMemScope() == "Parameter")
    {
        if (isVar(node))
        {
            node->setMemLoc(s_foffsets.back());
            s_foffsets.back() -= node->getMemSize();
        }
    }

    // if (!isVar(node))
    // {
        std::vector<Node *> children = node->getChildren();
        for (int i = 0; i < children.size(); i++)
        {
            symTableInitialize(children[i]);
        }
    // }

    switch (node->getNodeKind())
    {
        case Node::Kind::Func:
        {
            Func *func = (Func *)node;
            node->setMemSize(func->getSize()); // need to get function size
            break;
        }
        case Node::Kind::Compound:
        case Node::Kind::For:
            if (s_foffsets.size() > 0)
            {
                node->setMemSize(s_foffsets.back());
            }
            break;
    }

    symTableLeaveScope(node, false);
    if (hasScope)
    {
        s_foffsets.pop_back();
        if (isFor(node))
        {
            node->setMemSize(s_foffsets.back() - 1);
        }
    }

    symTableInitialize(node->getSibling());
}

Data * Semantics::symTableSetType(Node *node)
{
    if (!isExp(node))
    {
        return new Data(Data::Type::Undefined, false, false);
    }

    Exp *exp = (Exp *)node;
    switch (exp->getNodeKind())
    {
        case Node::Kind::Asgn:
        {
            Asgn *asgn = (Asgn *)exp;
            if (asgn->getType() == Asgn::Type::Asgn)
            {
                exp->setData(symTableSetType(asgn->getChild()));
            }
            else if (symTableSetType(asgn->getChild())->getType() == Data::Type::Undefined && symTableSetType(asgn->getChild(1))->getType() == Data::Type::Undefined)
            {
                exp->setData(new Data(Data::Type::Undefined, false, false));
            }
        }
        case Node::Kind::Binary:
        {
            Binary *binary = (Binary *)exp;
            if (binary->getType() == Binary::Type::Index)
            {
                exp->setData(symTableSetType(binary->getChild())->getNextData());
            }
            else if (symTableSetType(binary->getChild())->getType() == Data::Type::Undefined && symTableSetType(binary->getChild(1))->getType() == Data::Type::Undefined)
            {
                exp->setData(new Data(Data::Type::Undefined, false, false));
            }
            break;
        }
        case Node::Kind::Call:
        {
            Decl *decl = symTableGet(((Call *)node)->getName());
            if (isFunc(decl))
            {
                exp->setData(decl->getData());
            }
            break;
        }
        case Node::Kind::Id:
        {
            Decl *decl = symTableGet(((Id *)node)->getName());
            if (isVar(decl) || isParm(decl))
            {
                exp->setData(decl->getData());
            }
            // exp->setMemSize(decl->getMemSize());
            // exp->setMemLoc(decl->getMemLoc());
            // exp->setMemScope(decl->getMemScope());
            break;
        }
        case Node::Kind::Unary:
        {
            Unary *unary = (Unary *)exp;
            if (symTableSetType(unary->getChild())->getType() == Data::Type::Undefined)
            {
                exp->setData(new Data(Data::Type::Undefined, false, false));
            }
            break;
        }
    }
    return exp->getData();
}

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

bool Semantics::symTableEnterScope(const Node *node)
{
    if (node->getNodeKind() == Node::Kind::Compound)
    {
        if (node->getParent() != nullptr)
        {
            Node::Kind parentKind = node->getParent()->getNodeKind();
            if (parentKind == Node::Kind::For || parentKind == Node::Kind::Func)
            {
                return false;
            }
        }
        symTableSimpleEnterScope("Compound statement");
        return true;
    }

    switch (node->getNodeKind())
    {
        case Node::Kind::For:
            symTableSimpleEnterScope("For loop");
            return true;
            break;
        case Node::Kind::Func:
            symTableSimpleEnterScope("Function body");
            return true;
            break;
    }

    return false;
}

bool Semantics::symTableLeaveScope(const Node *node, const bool showWarns)
{
    Node::Kind nodeKind = node->getNodeKind();
    if (nodeKind == Node::Kind::For)
    {
        symTableSimpleLeaveScope(showWarns);
        return true;
    }
    else if (nodeKind == Node::Kind::Func)
    {
        Func *func = (Func *)node;
        if (showWarns && func->getData()->getType() != Data::Type::Undefined && func->getData()->getType() != Data::Type::Void && !func->getHasReturn())
        {
            Emit::warn(func->getLineNum(), "Expecting to return type " + func->getData()->stringify() + " but function '" + func->getName() + "' has no return statement.");
        }
        symTableSimpleLeaveScope(showWarns);
        return true;
    }

    if (node->getParent() != nullptr)
    {
        Node::Kind parentKind = node->getParent()->getNodeKind();
        if (nodeKind == Node::Kind::Compound && (parentKind != Node::Kind::For && parentKind != Node::Kind::Func))
        {
            // Func *func = (Func *)node;
            // if (showWarns && isFunc(func) && !func->getHasReturn())
            // {
            //     Emit::error(func->getLineNum(), "Expecting to return type " + func->getData()->stringify() + " but function '" + func->getName() + "' has no return statement.");
            // }
            symTableSimpleLeaveScope(showWarns);
            return true;
        }
    }
    else
    {
        if (nodeKind == Node::Kind::Compound)
        {
            symTableSimpleLeaveScope(showWarns);
            return true;
        }
    }

    return false;
}

void Semantics::symTableInitializeIOTree()
{
    Func *outputFunc = new Func(-1, "output", new Data(Data::Type::Void, false, false));
    Parm *outputParm = new Parm(-1, "*dummy1*", new Data(Data::Type::Int, false, false));
    outputFunc->makeUsed();
    outputParm->makeUsed();
    outputFunc->addChild(outputParm);

    Func *outputbFunc = new Func(-1, "outputb", new Data(Data::Type::Void, false, false));
    Parm *outputbParm = new Parm(-1, "*dummy2*", new Data(Data::Type::Bool, false, false));
    outputbFunc->makeUsed();
    outputbParm->makeUsed();
    outputbFunc->addChild(outputbParm);

    Func *outputcFunc = new Func(-1, "outputc", new Data(Data::Type::Void, false, false));
    Parm *outputcParm = new Parm(-1, "*dummy3*", new Data(Data::Type::Char, false, false));
    outputcFunc->makeUsed();
    outputcParm->makeUsed();
    outputcFunc->addChild(outputcParm);

    Func *inputFunc = new Func(-1, "input", new Data(Data::Type::Int, false, false));
    inputFunc->makeUsed();
    inputFunc->makeHasReturn();

    Func *inputbFunc = new Func(-1, "inputb", new Data(Data::Type::Bool, false, false));
    inputbFunc->makeUsed();
    inputbFunc->makeHasReturn();

    Func *inputcFunc = new Func(-1, "inputc", new Data(Data::Type::Char, false, false));
    inputcFunc->makeUsed();
    inputcFunc->makeHasReturn();

    Func *outnlFunc = new Func(-1, "outnl", new Data(Data::Type::Void, false, false));
    outnlFunc->makeUsed();

    outputFunc->addSibling(outputbFunc);
    outputFunc->addSibling(outputcFunc);
    outputFunc->addSibling(inputFunc);
    outputFunc->addSibling(inputbFunc);
    outputFunc->addSibling(inputcFunc);
    outputFunc->addSibling(outnlFunc);

    m_ioRoot = outputFunc;
}

void Semantics::symTableInjectIOTree(Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    if (isDecl(node))
    {
        symTableInsert((Decl *)node, false);
    }

    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        symTableInjectIOTree(children[i]);
    }
    symTableInjectIOTree(node->getSibling());
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

bool Semantics::hasIndexRelative(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::hasIndexRelative() - Invalid Exp");
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

bool Semantics::hasAsgnRelative(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::hasAsgnRelative() - Invalid Exp");
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

bool Semantics::hasNonConstantRelative(const Exp *exp) const
{
    if (!isExp(exp))
    {
        throw std::runtime_error("Semantics::hasNonConstantRelative() - Invalid Exp");
    }

    if (isUnary(exp))
    {
        Unary *unary = (Unary *)(exp);
        if (unary->getType() == Unary::Type::Question)
        {
            return true;
        }
    }
    else if (isCall(exp) || isId(exp))
    {
        return true;
    }

    std::vector<Node *> children = exp->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        if (hasNonConstantRelative((Exp *)children[i]))
        {
            return true;
        }
    }

    // Node *child = (Node *)exp;
    // while (child != nullptr)
    // {
    //     Emit::error(child->getLineNum(), child->stringifyWithType());
    //     if (isUnary(child))
    //     {
    //         Unary *unary = (Unary *)child;
    //         if (unary->getType() == Unary::Type::Question)
    //         {
    //             return true;
    //         }
    //     }
    //     else if (isCall(child) || isId(child))
    //     {
    //         return true;
    //     }
    //     child = child->getChild();
    // }

    return false;
}
