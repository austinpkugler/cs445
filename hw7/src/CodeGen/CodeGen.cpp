#include "CodeGen.hpp"

FILE *code = NULL;

CodeGen::CodeGen(Node *root, const std::string tmPath) : m_root(root), m_tmPath(tmPath), m_mainHasReturn(false), m_goffset(0), m_litOffset(1)
{
    m_toffsets.push_back(0);
}

CodeGen::~CodeGen()
{
    fclose(code);
}

void CodeGen::updateForMem(Node *node, std::vector<std::string> iterators)
{
    /*
     * This function is the result of ~5 hours of attempting to match the changes in hw6 memory management that suddenly appeared in hw7.
     * It is quite frankly disgusting code and should be deleted. However, it does help pass a few more test cases....
     * TODO:
     * k03.c-   Don't decrement when iterator var is in for loop compound
     * k07.c-   Handle operations in the range and the "by 2*c" portion
    */
    if (node == nullptr || node->getMemIsUpdated())
    {
        return;
    }

    node->setMemIsUpdated(true);

    if (isRange(node))
    {
        std::vector<Node *> children = node->getChildren();
        for (int i = 0; i < children.size(); i++)
        {
            if (isId(children[i]))
            {
                Id *id = (Id *)(children[i]);
                iterators.push_back(id->getName());
                id->setMemIsUpdated(true);
            }
        }
        updateForMem(node->getParent()->getChild(2), iterators);
    }
    // If it is a for loop then dec mem size and track the iterator
    else if (isFor(node))
    {
        node->setMemSize(node->getMemSize() - 2);
        Var *iterator = (Var *)(node->getChild());
        iterators.push_back(iterator->getName());
        iterator->setMemIsUpdated(true);
    }

    // If it inside a for loop then dec mem size
    Node *firstForRelative = node->getRelative(Node::Kind::For);
    if (!isVar(node) && !isId(node) && node->getMemExists() && firstForRelative != nullptr) // !isVar(node) && !isId(node) && 
    {
        node->setMemSize(node->getMemSize() - 2);

        // If it is inside 2 or more compounds then dec mem size
        Node *firstCompoundRelative = node->getRelative(Node::Kind::Compound);
        if (!isFor(node) && firstCompoundRelative != nullptr && firstCompoundRelative->hasRelative(Node::Kind::Compound))
        {
            node->setMemSize(node->getMemSize() - 2);
        }
    }

    // If it is inside 2 or more for loops then check if it is the iterator
    if (firstForRelative != nullptr && firstForRelative->hasRelative(Node::Kind::For))
    {
        // If the node is a reference to the iterator then dec mem loc
        if (isId(node))
        {
            Id *iteratorRef = (Id *)node;
            if (std::find(iterators.begin(), iterators.end(), iteratorRef->getName()) != iterators.end())
            {
                iteratorRef->setMemLoc(iteratorRef->getMemLoc() - 2);
            }
        }
        // If the node is the iterator then dec mem loc
        else if (isVar(node))
        {
            node->setMemLoc(node->getMemLoc() - 2);
        }
    }
    else if (firstForRelative != nullptr)
    {
        // If it is inside a for compound and not the iterator then dec mem loc
        Node *firstCompoundRelative = node->getRelative(Node::Kind::Compound);
        if (isFor(firstCompoundRelative->getParent()) && node->hasRelative(Node::Kind::Compound))
        {
            if (isId(node))
            {
                Id *iteratorRef = (Id *)node;
                if (std::find(iterators.begin(), iterators.end(), iteratorRef->getName()) == iterators.end())
                {
                    iteratorRef->setMemLoc(iteratorRef->getMemLoc() - 2);
                }
            }
            else if (isVar(node))
            {
                node->setMemLoc(node->getMemLoc() - 2);
            }
        }
    }

    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        updateForMem(children[i], iterators);
    }

    updateForMem(node->getSibling(), iterators);
}

void CodeGen::generate()
{
    if (m_root == nullptr)
    {
        return;
    }

    if (!(code = fopen(m_tmPath.c_str(), "w")))
    {
        throw std::runtime_error("CodeGen::generate() - Invalid tmPath provided to constructor");
    }

    std::vector<std::string> iterators;
    updateForMem(m_root, iterators);

    m_funcs["input"] = 1;
    m_funcs["output"] = 6;
    m_funcs["inputb"] = 12;
    m_funcs["outputb"] = 17;
    m_funcs["inputc"] = 23;
    m_funcs["outputc"] = 28;
    m_funcs["outnl"] = 34;
    emitIO();
    emitNewLoc(39);
    generateAndTraverse(m_root);
    int prevInstLoc = emitWhereAmI();
    emitNewLoc(0);
    emitRM("JMP", 7, prevInstLoc - 1, 7, "Jump to init [backpatch]");
    emitNewLoc(prevInstLoc);
    generateGlobals();
    emitRM("LDA", 3, 1, 7, "Return address in ac");
    emitRM("JMP", 7, -(emitWhereAmI() + 1 - m_funcs["main"]), 7, "Jump to main");
    emitRO("HALT", 0, 0, 0, "DONE!");
}

void CodeGen::sortGlobals()
{
    sort(m_globals.begin( ), m_globals.end( ), [ ]( const auto &lhs, const auto &rhs )
    {
    return lhs->getName() < rhs->getName();
    });
}

void CodeGen::generateGlobals()
{
    emitRM("LDA", 1, m_goffset, 0, "set first frame at end of globals");
    emitRM("ST", 1, 0, 1, "store old fp (point to self)");

    sortGlobals();
    for (int i = 0; i < m_globals.size(); i++)
    {
        generateNode(m_globals[i], true);
    }
}

void CodeGen::generateAndTraverse(Node *node, const bool generateGlobals)
{
    if (node == nullptr)
    {
        return;
    }

    generateNode(node, generateGlobals);

    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        generateAndTraverse(children[i]);
    }

    generateEnd(node);
    generateAndTraverse(node->getSibling());
}

void CodeGen::generateNode(Node *node, const bool generateGlobals)
{
    if (node == nullptr || node->getIsGenerated())
    {
        return;
    }

    // Return if we are not generating globals yet
    if (isVar(node))
    {
        Var *var = (Var *)node;
        if (var->getIsGlobal() && !generateGlobals)
        {
            m_globals.push_back(var);
            m_goffset -= var->getMemSize();
            return;
        }
    }
    else if (isConst(node))
    {
        Var *parent = (Var *)(node->getParent());
        if (isVar(parent) && parent->getIsGlobal() && !generateGlobals)
        {
            return;
        }
    }

    switch (node->getNodeKind())
    {
        case Node::Kind::Func:
            generateFunc((Func *)node);
            break;
        case Node::Kind::Parm:
            generateParm((Parm *)node);
            break;
        case Node::Kind::Var:
            generateVar((Var *)node, generateGlobals);
            break;
        case Node::Kind::Asgn:
            generateAsgn((Asgn *)node);
            break;
        case Node::Kind::Binary:
            generateBinary((Binary *)node);
            break;
        case Node::Kind::Call:
            generateCall((Call *)node);
            break;
        case Node::Kind::Const:
            generateConst((Const *)node);
            break;
        case Node::Kind::Id:
            generateId((Id *)node);
            break;
        case Node::Kind::Unary:
            generateUnary((Unary *)node);
            break;
        case Node::Kind::UnaryAsgn:
            generateUnaryAsgn((UnaryAsgn *)node);
            break;
        case Node::Kind::Break:
            generateBreak((Break *)node);
            break;
        case Node::Kind::Compound:
            generateCompound((Compound *)node);
            break;
        case Node::Kind::For:
            generateFor((For *)node);
            break;
        case Node::Kind::If:
            generateIf((If *)node);
            break;
        case Node::Kind::Range:
            generateRange((Range *)node);
            break;
        case Node::Kind::Return:
            generateReturn((Return *)node);
            break;
        case Node::Kind::While:
            generateWhile((While *)node);
            break;
    }

    node->makeGenerated();
}

void CodeGen::generateFunc(Func *func)
{
    emitRM("ST", 3, -1, 1, "Store return address");
    m_funcs[func->getName()] = emitWhereAmI() - 1;
    m_toffsets.back() -= 2;
}

void CodeGen::generateParm(Parm *parm)
{
    m_toffsets.back() -= parm->getMemSize();
}

void CodeGen::generateVar(Var *var, const bool generateGlobals)
{
    if (var->getData()->getIsArray())
    {
        emitRM("LDC", 3, var->getMemSize() - 1, 6, "load size of array", toChar(var->getName()));
        emitRM("ST", 3, var->getMemLoc() + 1, !var->getIsGlobal(), "save size of array", toChar(var->getName()));
    }

    if (var->getData()->getIsArray() && var->getData()->getType() == Data::Type::Char)
    {
        return;
    }

    // Special case for : assignment
    Node *varValue = var->getChild();
    if (varValue != nullptr)
    {
        generateAndTraverse(varValue, generateGlobals);
        emitRM("ST", 3, var->getMemLoc(), !var->getIsGlobal(), "Store variable", toChar(var->getName()));
    }
}

void CodeGen::generateAsgn(Asgn *asgn)
{
    Node *lhs = asgn->getChild();
    Node *rhs = asgn->getChild(1);
    if (isId(lhs))
    {
        generateAndTraverse(rhs);

        Id *id = (Id *)lhs;
        if (asgn->getType() != Asgn::Type::Asgn)
        {
            emitRM("LD", 4, id->getMemLoc(), !id->getIsGlobal(), "load lhs variable", toChar(id->getName()));
            emitRO(toChar(asgn->getTypeString()), 3, 4, 3, toChar("op " + asgn->getSym()));
        }
        else
        {
            if (id->getData()->getIsArray())
            {
                emitRM("LDA", 4, id->getMemLoc(), 1, "address of lhs");
                emitRM("LD", 5, 1, 3, "size of rhs");
                emitRM("LD", 6, 1, 4, "size of lhs");
                emitRO("SWP", 5, 6, 6, "pick smallest size");
                emitRO("MOV", 4, 3, 5, "array op =");
            }
        }

        if (!(id->getData()->getIsArray() && id->getData()->getType() == Data::Type::Char))
        {
            emitRM("ST", 3, id->getMemLoc(), !id->getIsGlobal(), "Store variable", toChar(id->getName()));
        }
    }
    else if (isBinary(lhs))
    {
        generateBinaryIndexValue((Binary *)lhs, rhs);
        Id *arrayId = (Id *)(lhs->getChild());
        if (asgn->getType() != Asgn::Type::Asgn)
        {
            emitRM("LD", 4, 0, 5, "load lhs variable", toChar(arrayId->getName()));
            emitRO(toChar(asgn->getTypeString()), 3, 4, 3, toChar("op " + asgn->getSym()));
        }
        emitRM("ST", 3, 0, 5, "Store variable", toChar(arrayId->getName()));
    }
}

void CodeGen::generateBinary(Binary *binary)
{
    if (binary->getType() != Binary::Type::Index)
    {
        Node *lhs = binary->getChild();
        generateAndTraverse(lhs);

        emitRM("ST", 3, m_toffsets.back(), 1, "Push left side");
        m_toffsets.back() -= 1;
        Node *rhs = binary->getChild(1);
        generateAndTraverse(rhs);
        m_toffsets.back() += 1;
        emitRM("LD", 4, m_toffsets.back(), 1, "Pop left into ac1");

        if (binary->getIsComparison())
        {
            Id *lhsArrayId = (Id *)lhs;
            Id *rhsArrayId = (Id *)rhs;
            if (isId(lhsArrayId) && lhsArrayId->getData()->getIsArray() && isId(rhsArrayId) && rhsArrayId->getData()->getIsArray())
            {
                emitRM("LD", 5, 1, 3, "AC2 <- |RHS|");
                emitRM("LD", 6, 1, 4, "AC3 <- |LHS|");
                emitRM("LDA", 2, 0, 5, "R2 <- |RHS|");
                emitRO("SWP", 5, 6, 6, "pick smallest size");
                emitRM("LD", 6, 1, 4, "AC3 <- |LHS|");
                emitRO("CO", 4, 3, 5, "setup array compare  LHS vs RHS");
                emitRO("TNE", 5, 4, 3, "if not equal then test (AC1, AC)");
                emitRO("JNZ", 5, 2, 7, "jump not equal");
                emitRM("LDA", 3, 0, 2, "AC1 <- |RHS|");
                emitRM("LDA", 4, 0, 6, "AC <- |LHS|");
            }
        }
        emitRO(toChar(binary->getTypeString()), 3, 4, 3, toChar("Op " + toUpper(binary->getSym())));
    }
    else
    {
        generateBinaryIndex(binary);
    }
}

void CodeGen::generateBinaryIndex(Binary *binary)
{
    if (binary->getType() != Binary::Type::Index)
    {
        throw std::runtime_error("CodeGen::generate() - Invalid tmPath provided to constructor");
    }

    Id *id = (Id *)(binary->getChild());
    if (id->getMemScope() == "Parameter")
    {
        emitRM("LD", 3, id->getMemLoc(), !id->getIsGlobal(), "Load address of base of array", toChar(id->getName()));
    }
    else
    {
        emitRM("LDA", 3, id->getMemLoc(), !id->getIsGlobal(), "Load address of base of array", toChar(id->getName()));
    }

    id->makeGenerated();
    binary->makeGenerated();
    emitRM("ST", 3, m_toffsets.back(), 1, "Push left side");
    m_toffsets.back() -= 1;
    generateAndTraverse(binary->getChild(1));
    m_toffsets.back() += 1;
    emitRM("LD", 4, m_toffsets.back(), 1, "Pop left into ac1");
    emitRO("SUB", 3, 4, 3, "compute location from index");
    emitRM("LD", 3, 0, 3, "Load array element");
}

void CodeGen::generateBinaryIndexValue(Binary *binary, Node *indexValue, int valueOffset3)
{
    if (binary->getType() != Binary::Type::Index)
    {
        throw std::runtime_error("CodeGen::generate() - Invalid tmPath provided to constructor");
    }

    Id *id = (Id *)(binary->getChild());
    generateAndTraverse(binary->getChild(1));

    if (indexValue != nullptr)
    {
        emitRM("ST", 3, m_toffsets.back(), 1, "Push index");
        m_toffsets.back() -= 1;
        generateAndTraverse(indexValue);
        m_toffsets.back() += 1;
        emitRM("LD", 4, m_toffsets.back(), 1, "Pop index");
    }

    if (id->getMemScope() == "Parameter")
    {
        emitRM("LD", 5, id->getMemLoc(), !id->getIsGlobal(), "Load address of base of array", toChar(id->getName()));
    }
    else
    {
        emitRM("LDA", 5, id->getMemLoc(), !id->getIsGlobal(), "Load address of base of array", toChar(id->getName()));
    }

    emitRO("SUB", 5, 5, valueOffset3, "Compute offset of value");

    id->makeGenerated();
    binary->makeGenerated();
}

void CodeGen::generateCall(Call *call)
{
    int prevToffset = m_toffsets.back();
    emitRM("ST", 1, m_toffsets.back(), 1, "Store fp in ghost frame for", toChar(call->getName()));
    m_toffsets.back() -= 2;

    std::vector<Node *> parms = call->getParms();
    for (int i = 0; i < parms.size(); i++)
    {
        generateNode(parms[i]);
        emitRM("ST", 3, m_toffsets.back(), 1, "Push parameter");
        m_toffsets.back() -= 1;
    }

    emitRM("LDA", 1, prevToffset, 1, "Ghost frame becomes new active frame");
    emitRM("LDA", 3, 1, 7, "Return address in ac");
    emitRM("JMP", 7, -(emitWhereAmI() + 1 - m_funcs[call->getName()]), 7, "CALL", toChar(call->getName()));
    emitRM("LDA", 3, 0, 2, "Save the result in ac");
    m_toffsets.back() = prevToffset;
}

void CodeGen::generateConst(Const *constN)
{
    switch (constN->getType())
    {
        case Const::Type::Int:
            emitRM("LDC", 3, constN->getIntValue(), 6, "Load integer constant");
            break;
        case Const::Type::Bool:
            emitRM("LDC", 3, constN->getBoolValue(), 6, "Load Boolean constant");
            break;
        case Const::Type::Char:
            emitRM("LDC", 3, (int)(constN->getCharValue()), 6, "Load char constant");
            break;
        case Const::Type::String:
            emitStrLit(m_litOffset, toChar(constN->getStringValue()));
            Node *parent = constN->getParent();
            emitRM("LDA", 3, constN->getMemLoc(), 0, "Load address of char array");
            if (!isCall(parent))
            {
                emitRM("LDA", 4, parent->getMemLoc(), 1, "address of lhs");
                emitRM("LD", 5, 1, 3, "size of rhs");
                emitRM("LD", 6, 1, 4, "size of lhs");
                emitRO("SWP", 5, 6, 6, "pick smallest size");
                emitRO("MOV", 4, 3, 5, "array op =");
            }
            m_litOffset += constN->getMemSize();
            m_goffset -= constN->getMemSize();
            break;
    }
}

void CodeGen::generateId(Id *id)
{
    Asgn *asgn = (Asgn *)(id->getRelative(Node::Kind::Asgn));
    if (asgn != nullptr && asgn->getChild() == id)
    {
        return;
    }

    if (id->getData()->getIsArray())
    {
        if (id->getMemScope() == "Parameter")
        {
            emitRM("LD", 3, id->getMemLoc(), !id->getIsGlobal(), "Load address of base of array", toChar(id->getName()));
        }
        else
        {
            emitRM("LDA", 3, id->getMemLoc(), !id->getIsGlobal(), "Load address of base of array", toChar(id->getName()));
        }
        id->makeGenerated();
        return;
    }

    if (id->getIsGlobal() || id->getData()->getIsStatic())
    {
        emitRM("LD", 3, id->getMemLoc(), 0, "Load variable", toChar(id->getName()));
    }
    else
    {
        emitRM("LD", 3, id->getMemLoc(), 1, "Load variable", toChar(id->getName()));
    }
}

void CodeGen::generateUnary(Unary *unary)
{
    switch (unary->getType())
    {
        case Unary::Type::Chsign:
            generateAndTraverse(unary->getChild());
            emitRO("NEG", 3, 3, 3, "Op unary -");
            break;
        case Unary::Type::Sizeof:
        {
            Id *id = (Id *)(unary->getChild());
            if (id->getMemScope() == "Parameter")
            {
                emitRM("LD", 3, id->getMemLoc(), !id->getIsGlobal(), "Load address of base of array", toChar(id->getName()));
            }
            else
            {
                emitRM("LDA", 3, id->getMemLoc(), !id->getIsGlobal(), "Load address of base of array", toChar(id->getName()));
            }
            emitRM("LD", 3, 1, 3, "Load array size");
            id->makeGenerated();
            break;
        }
        case Unary::Type::Question:
            generateAndTraverse(unary->getChild());
            emitRO("RND", 3, 3, 6, "Op ?");
            break;
        case Unary::Type::Not:
        {
            Node *lhs = unary->getChild();
            generateAndTraverse(lhs);
            emitRM("LDC", 4, 1, 6, "Load 1");
            emitRO("XOR", 3, 3, 4, "Op XOR to get logical not");
            break;
        }
    }
}

void CodeGen::generateUnaryAsgn(UnaryAsgn *unaryAsgn)
{
    Node *lhs = unaryAsgn->getChild();
    if (isId(lhs))
    {
        Id *id = (Id *)lhs;
        emitRM("LD", 3, id->getMemLoc(), !id->getIsGlobal(), "load lhs variable", toChar(id->getName()));
        emitRM("LDA", 3, unaryAsgn->getTypeValue(), 3, toChar(unaryAsgn->getTypeString() + " value of"), toChar(id->getName()));
        emitRM("ST", 3, id->getMemLoc(), !id->getIsGlobal(), "Store variable", toChar(id->getName()));
        id->makeGenerated();
    }
    else
    {
        generateBinaryIndexValue((Binary *)(unaryAsgn->getChild()), unaryAsgn->getChild(1), 3);

        Id *id = (Id *)(unaryAsgn->getChild()->getChild());
        emitRM("LD", 3, 0, 5, "load lhs variable", toChar(id->getName()));
        emitRM("LDA", 3, unaryAsgn->getTypeValue(), 3, toChar(unaryAsgn->getTypeString() + " value of"), toChar(id->getName()));
        emitRM("ST", 3, 0, 5, "Store variable", toChar(id->getName()));
    }
}

void CodeGen::generateBreak(Break *breakN)
{
    emitRM("JMP", 7, m_loffsets.back() - emitWhereAmI() - 1, 7, "break");
}

void CodeGen::generateCompound(Compound *compound)
{
    Node *currSibling = compound->getChild();
    while (currSibling != nullptr)
    {
        Var *var = (Var *)(currSibling);
        if (isVar(var) && !var->getData()->getIsStatic())
        {
            m_toffsets.back() -= var->getMemSize();
        }
        currSibling = currSibling->getSibling();
    }
}

void CodeGen::generateFor(For *forN)
{
    Range *range = (Range *)(forN->getChild(1));
    int prevInstLoc = m_toffsets.back();
    m_toffsets.push_back(m_toffsets.back() - 3);
    generateAndTraverse(range->getChild());
    emitRM("ST", 3, prevInstLoc, 1, "save starting value in index variable");
    generateAndTraverse(range->getChild(1));
    emitRM("ST", 3, prevInstLoc - 1, 1, "save stop value");
    if (range->getChild(2))
    {
        generateAndTraverse(range->getChild(2));
    }
    else
    {
        emitRM("LDC", 3, 1, 6, "default increment by 1");
    }
    emitRM("ST", 3, prevInstLoc - 2, 1, "save step value");

    int prevInstLoc2 = emitWhereAmI();
    emitRM("LD", 4, prevInstLoc, 1, "loop index");
    emitRM("LD", 5, prevInstLoc - 1, 1, "stop value");
    emitRM("LD", 3, prevInstLoc - 2, 1, "step value");
    emitRO("SLT", 3, 4, 5, "Op <");
    emitRM("JNZ", 3, 1, 7, "Jump to loop body");

    int prevInstLoc3 = emitWhereAmI();
    emitNewLoc(prevInstLoc3 + 1);
    generateAndTraverse(forN->getChild(2));
    emitRM("LD", 3, prevInstLoc, 1, "Load index");
    emitRM("LD", 5, prevInstLoc - 2, 1, "Load step");
    emitRO("ADD", 3, 3, 5, "increment");
    emitRM("ST", 3, prevInstLoc, 1, "store back to index");
    emitRM("JMP", 7, prevInstLoc2 - emitWhereAmI() - 1, 7, "go to beginning of loop");

    int prevInstLoc4 = emitWhereAmI();
    emitNewLoc(prevInstLoc3);
    emitRM("JMP", 7, prevInstLoc4 - prevInstLoc3 - 1, 7, "Jump past loop [backpatch]");
    emitNewLoc(prevInstLoc4);
    m_toffsets.pop_back();
}

void CodeGen::generateIf(If *ifN)
{
    // Generate lhs
    generateAndTraverse(ifN->getChild());
    int prevInstLoc = emitWhereAmI();
    emitNewLoc(prevInstLoc + 1);

    // Generate rhs
    generateAndTraverse(ifN->getChild(1));
    int prevInstLoc2 = emitWhereAmI();
    emitNewLoc(prevInstLoc);

    if (ifN->getChild(2) != nullptr)
    {
        // Handle the "then" statement
        emitRM("JZR", 3, prevInstLoc2 - prevInstLoc, 7, "Jump around the THEN if false [backpatch]");
        emitNewLoc(prevInstLoc2);
        prevInstLoc = emitWhereAmI();
        emitNewLoc(prevInstLoc + 1);
        generateAndTraverse(ifN->getChild(2));
        int prevInstLoc3 = emitWhereAmI();
        emitNewLoc(prevInstLoc);
        emitRM("JMP", 7, prevInstLoc3 - prevInstLoc - 1, 7, "Jump around the ELSE [backpatch]");
        emitNewLoc(prevInstLoc3);
    }
    else
    {
        // There is no "then" statement
        emitRM("JZR", 3, prevInstLoc2 - prevInstLoc - 1, 7, "Jump around the THEN if false [backpatch]");
        emitNewLoc(prevInstLoc2);
    }
}

void CodeGen::generateRange(Range *range)
{
    // Maybe handle the k01.c-,  k02.c-, etc. cases here
}

void CodeGen::generateReturn(Return *returnN)
{
    Node *lhs = returnN->getChild();
    if (lhs != nullptr)
    {
        generateAndTraverse(lhs);
        emitRM("LDA", 2, 0, 3, "Copy result to return register");
    }

    emitRM("LD", 3, -1, 1, "Load return address");
    emitRM("LD", 1, 0, 1, "Adjust fp");
    emitRM("JMP", 7, 0, 3, "Return");

    Func *func = (Func *)(returnN->getRelative(Node::Kind::Func));
    if (isFunc(func) && func->getName() == "main")
    {
        m_mainHasReturn = true;
    }
}

void CodeGen::generateWhile(While *whileN)
{
    // Generate lhs
    int prevInstLoc = emitWhereAmI();
    generateAndTraverse(whileN->getChild());
    emitRM("JNZ", 3, 1, 7, "Jump to while part");

    // Save offset for While
    m_loffsets.push_back(emitWhereAmI());

    // Generate rhs
    int prevInstLoc2 = emitWhereAmI();
    emitNewLoc(prevInstLoc2 + 1);
    generateAndTraverse(whileN->getChild(1));
    emitRM("JMP", 7, prevInstLoc - emitWhereAmI() - 1, 7, "go to beginning of loop");

    int prevInstLoc3 = emitWhereAmI();
    emitNewLoc(prevInstLoc2);
    emitRM("JMP", 7, prevInstLoc3 - prevInstLoc2 - 1, 7, "Jump past loop [backpatch]");
    emitNewLoc(prevInstLoc3);

    m_loffsets.pop_back();
}

void CodeGen::generateEnd(Node *node)
{
    if (isFunc(node))
    {
        Func *func = (Func *)node;
        emitRM("LDC", 2, 0, 6, "Set return value to 0");
        emitRM("LD", 3, -1, 1, "Load return address");
        emitRM("LD", 1, 0, 1, "Adjust fp");
        emitRM("JMP", 7, 0, 3, "Return");
        int prevInstLoc = emitWhereAmI();
        emitNewLoc(0);
        emitNewLoc(prevInstLoc);
        m_toffsets.back() = 0;
    }
    else if (isCompound(node))
    {
        Node *currSibling = node->getChild();
        while (currSibling != nullptr)
        {
            Var *var = (Var *)(currSibling);
            if (isVar(var) && !var->getData()->getIsStatic())
            {
                m_toffsets.back() += var->getMemSize();
            }
            currSibling = currSibling->getSibling();
        }
    }
}
