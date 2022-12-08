#include "CodeGen.hpp"

FILE *code = NULL;

CodeGen::CodeGen(Node *root, const std::string tmPath, bool showLog) : m_root(root), m_tmPath(tmPath), m_showLog(showLog) {}

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

    log("\nCodeGen::generate()", "Starting generation", -1);

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
    generateGlobals();
    emitRM("LDA", 3, 1, 7, "Return address in ac");
    emitRM("JMP", 7, -(emitWhereAmI() + 1 - m_funcs["main"]), 7, "Jump to main");
    emitRO("HALT", 0, 0, 0, "DONE!");
}

void CodeGen::generateGlobals()
{
    emitRM("LDA", 1, m_goffset, 0, "set first frame at end of globals");
    emitRM("ST", 1, 0, 1, "store old fp (point to self)");
    for (int i = 0; i < m_globals.size(); i++)
    {
        generateAndTraverse(m_globals[i]->getChild());

        if (m_globals[i]->getData()->getIsArray())
        {
            emitRM("LDC", 3, m_globals[i]->getMemSize() - 1, 6, "load size of array", toChar(m_globals[i]->getName()));
            emitRM("ST", 3, 0, 0, "save size of array", toChar(m_globals[i]->getName()));
        }

        // if ()
        // {
        //     emitRM("ST", 3, -2, 0, "Store variable", toChar(m_globals[i]->getName()));
        // }
        // else if (m_globals[i]->getData()->getIsStatic())
        // {
        //     emitRM("ST", 3, m_globals[i]->getMemLoc(), 0, "Store variable", toChar(m_globals[i]->getName()));
            
        // }

        m_globals[i]->makeGenerated();
    }
}

void CodeGen::generateAndTraverse(Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    generateNode(node);

    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        generateAndTraverse(children[i]);
    }

    generateEnd(node);
    generateAndTraverse(node->getSibling());
}

void CodeGen::generateNode(Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    if (node->getIsGenerated())
    {
        return;
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
            generateVar((Var *)node);
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
    m_toffset -= 2;
    log("CodeGen::generateFunc()", "dec for Func TOFF: " + std::to_string(m_toffset), func->getLineNum());
}

void CodeGen::generateParm(Parm *parm)
{
    // if (parm->getData()->getIsArray())
    // {
    //     emitRM("LDC", 3, parm->getMemSize() - 1, 6, "load size of array", toChar(parm->getName()));
    //     emitRM("ST", 3, -2, 1, "save size of array", toChar(parm->getName()));
    // }
    m_toffset -= parm->getMemSize();
    log("CodeGen::generateParm()", "dec for Parm TOFF: " + std::to_string(m_toffset), parm->getLineNum());
}

void CodeGen::generateVar(Var *var)
{
    if (var->getIsGlobal())
    {
        m_globals.push_back(var);
        m_goffset -= var->getMemSize();
    }
    else
    {
        // Special case for : assignment
        Node *lhs = var->getChild();
        if (lhs != nullptr)
        {
            generateAndTraverse(lhs);
            emitRM("ST", 3, -2, 1, "Store variable", toChar(var->getName()));
        }

        m_toffset -= var->getMemSize();
        log("CodeGen::generateVar()", "dec for Var TOFF: " + std::to_string(m_toffset), var->getLineNum());
    }
}

void CodeGen::generateAsgn(Asgn *asgn)
{
    Node *rhs = asgn->getChild(1);

    log("CodeGen::generateAsgn()", "Asgn rhs is " + rhs->stringifyWithType(), rhs->getLineNum());
    generateAndTraverse(rhs);

    Node *lhs = asgn->getChild();
    if (isId(lhs))
    {
        Id *id = (Id *)lhs;
        if (id->getIsGlobal())
        {
            emitRM("ST", 3, id->getMemLoc(), 0, "Store variable", toChar(id->getName()));
        }
        else
        {
            emitRM("ST", 3, id->getMemLoc(), 1, "Store variable", toChar(id->getName()));
        }
    }
}

void CodeGen::generateBinary(Binary *binary)
{
    Node *lhs = binary->getChild();
    generateAndTraverse(lhs);
    emitRM("ST", 3, m_toffset, 1, "Push left side");

    Node *rhs = binary->getChild(1);
    m_toffset -= rhs->getMemSize();
    generateAndTraverse(rhs);
    m_toffset += rhs->getMemSize();
    emitRM("LD", 4, m_toffset, 1, "Pop left into ac1");

    emitRO(toChar(binary->getTypeString()), 3, 4, 3, toChar("Op " + toUpper(binary->getSym())));
}

void CodeGen::generateCall(Call *call)
{
    int prevToffset = m_toffset;
    emitRM("ST", 1, m_toffset, 1, "Store fp in ghost frame for", toChar(call->getName()));
    m_toffset -= 2;
    log("CodeGen::generateCall()", "dec for Call TOFF: " + std::to_string(m_toffset), call->getLineNum());

    std::vector<Node *> parms = call->getParms();
    for (int i = 0; i < parms.size(); i++)
    {
        log("CodeGen::generateCall()", "Generating param " + parms[i]->stringifyWithType() + " for " + call->stringifyWithType(), call->getLineNum());
        generateNode(parms[i]);
        log("CodeGen::generateCall()", "ST 3," + std::to_string(m_toffset) + "(1) Push parameter", call->getLineNum());
        emitRM("ST", 3, m_toffset, 1, "Push parameter");
        m_toffset -= parms[i]->getMemSize();
    }

    emitRM("LDA", 1, prevToffset, 1, "Ghost frame becomes new active frame");
    emitRM("LDA", 3, 1, 7, "Return address in ac");
    emitRM("JMP", 7, -(emitWhereAmI() + 1 - m_funcs[call->getName()]), 7, "CALL", toChar(call->getName()));
    emitRM("LDA", 3, 0, 2, "Save the result in ac");
    m_toffset = prevToffset;
    log("CodeGen::generateCall()", "reset for end of Call TOFF 2: " + std::to_string(m_toffset), call->getLineNum());
}

void CodeGen::generateConst(Const *constN)
{
    switch (constN->getType())
    {
        case Const::Type::Int:
            log("CodeGen::generateConst()", "LDC 3," + std::to_string(constN->getIntValue()) + "(6) Load integer constant", constN->getLineNum());
            emitRM("LDC", 3, constN->getIntValue(), 6, "Load integer constant");
            break;
        case Const::Type::Bool:
            emitRM("LDC", 3, constN->getBoolValue(), 6, "Load Boolean constant");
            break;
        case Const::Type::Char:
            emitRM("LDC", 3, (int)(constN->getCharValue()), 6, "Load char constant");
            break;
    }
}

void CodeGen::generateId(Id *id)
{
    // Only load the Id if it is on the rhs of =
    Asgn *asgn = (Asgn *)(id->getRelative(Node::Kind::Asgn));
    if (asgn != nullptr && asgn->getChild() == id)
    {
        return;
    }

    if (id->getIsGlobal() || id->getData()->getIsStatic())
    {
        log("CodeGen::generateId()", "LD 3," + std::to_string(id->getMemLoc()) + "(0) Load variable " + id->getName(), id->getLineNum());
        emitRM("LD", 3, id->getMemLoc(), 0, "Load variable", toChar(id->getName()));
    }
    else
    {
        log("CodeGen::generateId()", "LD 3," + std::to_string(id->getMemLoc()) + "(1) Load variable " + id->getName(), id->getLineNum());
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
            break;
        case Unary::Type::Question:
            emitRO("RND", 3, 3, 6, "Op ?");
            emitRM("ST", 3, m_toffset, 1, "Push left side");
            emitRM("LDC", 3, 0, 6, "Load integer constant");
            emitRM("LD", 4, m_toffset, 1, "Pop left into ac1");
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

}

void CodeGen::generateBreak(Break *breakN)
{

}

void CodeGen::generateCompound(Compound *compound)
{

}

void CodeGen::generateFor(For *forN)
{

}

void CodeGen::generateIf(If *ifN)
{

}

void CodeGen::generateRange(Range *range)
{

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
}

void CodeGen::generateWhile(While *whileN)
{

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
        int instCount = emitWhereAmI();
        emitNewLoc(0);
        if (func->getName() == "main")
        {
            emitRM("JMP", 7, instCount - 1, 7, "Jump to init [backpatch]");
        }
        emitNewLoc(instCount);
        m_toffset = 0;
        log("CodeGen::generateEnd()", "reset for end of generation TOFF: " + std::to_string(m_toffset), node->getLineNum());
    }
}

void CodeGen::log(const std::string loc, const std::string msg, const int lineNum)
{
    if (m_showLog)
    {
        std::cout << loc << " - " << emitWhereAmI() << ": " << msg << " (line " << lineNum << ")" << std::endl;
    }
}
