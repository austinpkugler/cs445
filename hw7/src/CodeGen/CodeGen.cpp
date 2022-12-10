#include "CodeGen.hpp"

FILE *code = NULL;

CodeGen::CodeGen(Node *root, const std::string tmPath, bool showLog) : m_root(root), m_tmPath(tmPath), m_showLog(showLog), m_mainHasReturn(false), m_goffset(0), m_litOffset(1)
{
    m_toffsets.push_back(0);
}

CodeGen::~CodeGen()
{
    fclose(code);
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

    log("Generating Node: " + node->stringifyWithType(), node->getLineNum());

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

    log("Done Generating Node", node->getLineNum());
}

void CodeGen::generateFunc(Func *func)
{
    log("enter generateFunc()", func->getLineNum());

    emitRM("ST", 3, -1, 1, "Store return address");
    m_funcs[func->getName()] = emitWhereAmI() - 1;
    m_toffsets.back() -= 2;
    log("generateFunc() TOFF dec for Func", func->getLineNum());

    log("leave generateFunc()", func->getLineNum());
}

void CodeGen::generateParm(Parm *parm)
{
    log("enter generateParm()", parm->getLineNum());

    m_toffsets.back() -= parm->getMemSize();
    log("generateParm()", parm->getLineNum());

    log("leave generateParm()", parm->getLineNum());
}

void CodeGen::generateVar(Var *var, const bool generateGlobals)
{
    log("enter generateVar()", var->getLineNum());

    if (var->getData()->getIsArray())
    {
        emitRM("LDC", 3, var->getMemSize() - 1, 6, "load size of array", toChar(var->getName()));
        emitRM("ST", 3, var->getMemLoc() + 1, !var->getIsGlobal(), "save size of array", toChar(var->getName()));
    }

    // Special case for : assignment
    Node *varValue = var->getChild();
    if (varValue != nullptr)
    {
        log("Var has : assignment and value is " + varValue->stringifyWithType(), var->getLineNum());
        generateAndTraverse(varValue, generateGlobals);

        // if (var->getData()->getIsArray() && isConst(varValue))
        // {
        //     Const *varValueConst = (Const *)varValue;
        //     if (varValueConst->getType() == Const::Type::String)
        //     {
        //         return;
        //     }
        // }
        emitRM("ST", 3, var->getMemLoc(), !var->getIsGlobal(), "Store variable", toChar(var->getName()));
    }

    log("leave generateVar()", var->getLineNum());
}

void CodeGen::generateAsgn(Asgn *asgn)
{
    log("enter generateAsgn()", asgn->getLineNum());

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
        emitRM("ST", 3, id->getMemLoc(), !id->getIsGlobal(), "Store variable", toChar(id->getName()));
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

    log("leave generateAsgn()", asgn->getLineNum());
}

void CodeGen::generateBinary(Binary *binary)
{
    log("enter generateBinary()", binary->getLineNum());

    if (binary->getType() != Binary::Type::Index)
    {
        generateAndTraverse(binary->getChild());

        log("generateBinary() Push left side", binary->getLineNum());
        emitRM("ST", 3, m_toffsets.back(), 1, "Push left side");

        m_toffsets.back() -= 1;
        log("generateBinary() TOFF dec before binary rhs", binary->getLineNum());

        generateAndTraverse(binary->getChild(1));

        m_toffsets.back() += 1;
        log("generateBinary() TOFF inc after binary rhs", binary->getLineNum());

        log("generateBinary() Pop left into ac1", binary->getLineNum());
        emitRM("LD", 4, m_toffsets.back(), 1, "Pop left into ac1");

        emitRO(toChar(binary->getTypeString()), 3, 4, 3, toChar("Op " + toUpper(binary->getSym())));
    }
    else
    {
        generateBinaryIndex(binary);
    }

    log("leave generateBinary()", binary->getLineNum());
}

void CodeGen::generateBinaryIndex(Binary *binary)
{
    log("enter generateBinaryIndex()", binary->getLineNum());

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
    log("generateBinaryIndex() TOFF dec before binary rhs", binary->getLineNum());
    generateAndTraverse(binary->getChild(1));
    m_toffsets.back() += 1;
    log("generateBinaryIndex() TOFF inc after binary rhs", binary->getLineNum());
    emitRM("LD", 4, m_toffsets.back(), 1, "Pop left into ac1");
    emitRO("SUB", 3, 4, 3, "compute location from index");
    emitRM("LD", 3, 0, 3, "Load array element");

    log("leave generateBinaryIndex()", binary->getLineNum());
}

void CodeGen::generateBinaryIndexValue(Binary *binary, Node *indexValue, int valueOffset3)
{
    log("enter generateBinaryIndexValue()", binary->getLineNum());

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
        log("generateBinaryIndexValue() TOFF dec before indexValue", binary->getLineNum());
        generateAndTraverse(indexValue);
        m_toffsets.back() += 1;
        log("generateBinaryIndexValue() TOFF inc after indexValue", binary->getLineNum());
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

    log("leave generateBinaryIndexValue()", binary->getLineNum());
}

void CodeGen::generateCall(Call *call)
{
    log("enter generateCall()", call->getLineNum());

    int prevToffset = m_toffsets.back();
    log("ST 1," + std::to_string(m_toffsets.back()) + "(1) Store fp in ghost frame for " + call->getName(), call->getLineNum());
    emitRM("ST", 1, m_toffsets.back(), 1, "Store fp in ghost frame for", toChar(call->getName()));
    m_toffsets.back() -= 2;
    log("generateCall() TOFF dec for call", call->getLineNum());

    std::vector<Node *> parms = call->getParms();
    for (int i = 0; i < parms.size(); i++)
    {
        generateNode(parms[i]);
        log("generateCall() ST 3," + std::to_string(m_toffsets.back()) + "(1) Push parameter", parms[i]->getLineNum());
        emitRM("ST", 3, m_toffsets.back(), 1, "Push parameter");
        m_toffsets.back() -= 1;
        log("generateCall() TOFF dec for parms[i]", parms[i]->getLineNum());
    }

    emitRM("LDA", 1, prevToffset, 1, "Ghost frame becomes new active frame");
    emitRM("LDA", 3, 1, 7, "Return address in ac");
    emitRM("JMP", 7, -(emitWhereAmI() + 1 - m_funcs[call->getName()]), 7, "CALL", toChar(call->getName()));
    emitRM("LDA", 3, 0, 2, "Save the result in ac");
    m_toffsets.back() = prevToffset;
    log("generateCall() TOFF reset at end of call", call->getLineNum());

    log("leave generateCall()", call->getLineNum());
}

void CodeGen::generateConst(Const *constN)
{
    log("enter generateConst()", constN->getLineNum());

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

            Node *id = constN->getParent();
            emitRM("LDA", 3, constN->getMemLoc(), 0, "Load address of char array");
            emitRM("LDA", 4, id->getMemLoc(), 1, "address of lhs");
            emitRM("LD", 5, 1, 3, "size of rhs");
            emitRM("LD", 6, 1, 4, "size of lhs");
            emitRO("SWP", 5, 6, 6, "pick smallest size");
            emitRO("MOV", 4, 3, 5, "array op =");
            m_litOffset += constN->getMemSize();
            break;
    }

    log("leave generateConst()", constN->getLineNum());
}

void CodeGen::generateId(Id *id)
{
    log("enter generateId()", id->getLineNum());

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

    Asgn *asgn = (Asgn *)(id->getRelative(Node::Kind::Asgn));
    if (asgn != nullptr && asgn->getChild() == id)
    {
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

    log("leave generateId()", id->getLineNum());
}

void CodeGen::generateUnary(Unary *unary)
{
    log("enter generateUnary()", unary->getLineNum());

    switch (unary->getType())
    {
        case Unary::Type::Chsign:
            generateAndTraverse(unary->getChild());
            emitRO("NEG", 3, 3, 3, "Op unary -");
            break;
        case Unary::Type::Sizeof:
        {
            Id *id = (Id *)(unary->getChild());
            emitRM("LDA", 3, id->getMemLoc(), !id->getIsGlobal(), "Load address of base of array", toChar(id->getName()));
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

    log("leave generateUnary()", unary->getLineNum());
}

void CodeGen::generateUnaryAsgn(UnaryAsgn *unaryAsgn)
{
    log("enter generateUnaryAsgn()", unaryAsgn->getLineNum());

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

    log("leave generateUnaryAsgn()", unaryAsgn->getLineNum());
}

void CodeGen::generateBreak(Break *breakN)
{
    log("enter generateBreak()", breakN->getLineNum());

    emitRM("JMP", 7, m_loffsets.back() - emitWhereAmI() - 1, 7, "break");

    log("leave generateBreak()", breakN->getLineNum());
}

void CodeGen::generateCompound(Compound *compound)
{
    log("enter generateCompound()", compound->getLineNum());

    Node *currSibling = compound->getChild();
    while (currSibling != nullptr)
    {
        Var *var = (Var *)(currSibling);
        if (isVar(var) && !var->getData()->getIsStatic())
        {
            m_toffsets.back() -= var->getMemSize();
            log("generateCompound() TOFF dec for compound var", var->getLineNum());
        }
        currSibling = currSibling->getSibling();
    }
    // m_toffsets.push_back(m_toffsets.back());

    log("leave generateCompound()", compound->getLineNum());
}

void CodeGen::generateFor(For *forN)
{
    log("enter generateFor()", forN->getLineNum());

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

    log("leave generateFor()", forN->getLineNum());
}

void CodeGen::generateIf(If *ifN)
{
    log("enter generateIf()", ifN->getLineNum());

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

    log("leave generateIf()", ifN->getLineNum());
}

void CodeGen::generateRange(Range *range)
{
    log("enter generateRange()", range->getLineNum());
    log("leave generateRange()", range->getLineNum());
}

void CodeGen::generateReturn(Return *returnN)
{
    log("enter generateReturn()", returnN->getLineNum());

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

    log("leave generateReturn()", returnN->getLineNum());
}

void CodeGen::generateWhile(While *whileN)
{
    log("enter generateWhile()", whileN->getLineNum());

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

    log("leave generateWhile()", whileN->getLineNum());
}

void CodeGen::generateEnd(Node *node)
{
    log("enter generateEnd()", node->getLineNum());

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
        log("generateEnd() reset at end of func", func->getLineNum());
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
                log("generateEnd() TOFF inc for compound var", var->getLineNum());
            }
            currSibling = currSibling->getSibling();
        }
        // m_toffsets.pop_back();
    }

    log("leave generateEnd()", node->getLineNum());
}

void CodeGen::log(const std::string msg, const int lineNum) const
{
    if (m_showLog)
    {
        std::cout << "line " << lineNum << ", toffset " << m_toffsets.back() << ", emit " << emitWhereAmI() << ": " << msg << std::endl;
    }
}

void CodeGen::logBreak() const
{
    if (m_showLog)
    {
        std::cout << std::endl;
    }
}
