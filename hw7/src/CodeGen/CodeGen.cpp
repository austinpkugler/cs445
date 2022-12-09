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
        generateNode(m_globals[i], true);
        // generateAndTraverse(m_globals[i]->getChild());

        // if (m_globals[i]->getData()->getIsArray())
        // {
        //     emitRM("LDC", 3, m_globals[i]->getMemSize() - 1, 6, "load size of array", toChar(m_globals[i]->getName()));
        //     emitRM("ST", 3, 0, 0, "save size of array", toChar(m_globals[i]->getName()));
        // }



        // if ()
        // {
        //     emitRM("ST", 3, -2, 0, "Store variable", toChar(m_globals[i]->getName()));
        // }
        // else if (m_globals[i]->getData()->getIsStatic())
        // {
        //     emitRM("ST", 3, m_globals[i]->getMemLoc(), 0, "Store variable", toChar(m_globals[i]->getName()));
            
        // }

        // m_globals[i]->makeGenerated();
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

    log("CodeGen::generateNode()", "Generating Node " + node->stringifyWithType(), node->getLineNum());

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
    m_toffset -= 2;
    log("CodeGen::generateFunc()", "TOFF: " + std::to_string(m_toffset), func->getLineNum());
}

void CodeGen::generateParm(Parm *parm)
{
    m_toffset -= parm->getMemSize();
    log("CodeGen::generateParm()", "TOFF: " + std::to_string(m_toffset), parm->getLineNum());
}

void CodeGen::generateVar(Var *var, const bool generateGlobals)
{
    log("CodeGen::generateVar()", "Generating Var " + var->stringifyWithType(), var->getLineNum());
    // Special case for : assignment
    Node *lhs = var->getChild();
    if (lhs != nullptr)
    {
        log("CodeGen::generateVar()", "Var has a child, it will be traversed", var->getLineNum());
        generateAndTraverse(lhs, generateGlobals);
        log("CodeGen::generateVar()", "Done traversing child", var->getLineNum());
        emitRM("ST", 3, var->getMemLoc(), !var->getIsGlobal(), "Store variable", toChar(var->getName()));
    }

    if (!var->getIsGlobal())
    {
        m_toffset -= var->getMemSize();
        log("CodeGen::generateVar()", "TOFF: " + std::to_string(m_toffset), var->getLineNum());
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
        if (asgn->getType() != Asgn::Type::Asgn)
        {
            emitRM("LD", 4, id->getMemLoc(), !id->getIsGlobal(), "load lhs variable", toChar(id->getName()));
            emitRO(toChar(asgn->getTypeString()), 3, 4, 3, toChar("op " + asgn->getSym()));
        }
        emitRM("ST", 3, id->getMemLoc(), !id->getIsGlobal(), "Store variable", toChar(id->getName()));
    }
    else if (isBinary(lhs))
    {
        Binary *binary = (Binary *)lhs;
        if (binary->getType() == Binary::Type::Index)
        {
            generateNode(binary->getChild(1));
            emitRM("ST", 3, m_toffset, 1, "Push index");
            generateNode(asgn->getChild(1));
            emitRM("ST", 3, m_toffset, 1, "Pop index");
        }
    }
}

void CodeGen::generateBinary(Binary *binary)
{
    if (binary->getType() != Binary::Type::Index)
    {
        Node *lhs = binary->getChild();
        generateNode(lhs);

        log("CodeGen::generateBinary()", "ST 3," + std::to_string(m_toffset) + "(1) Push left side", lhs->getLineNum());
        emitRM("ST", 3, m_toffset, 1, "Push left side");

        Node *rhs = binary->getChild(1);
        m_toffset -= rhs->getMemSize();
        log("CodeGen::generateBinary()", "TOFF: " + std::to_string(m_toffset), rhs->getLineNum());
        generateNode(rhs);

        

        m_toffset += rhs->getMemSize();
        log("CodeGen::generateBinary()", "TOFF 2: " + std::to_string(m_toffset), rhs->getLineNum());
        emitRM("LD", 4, m_toffset, 1, "Pop left into ac1");

        emitRO(toChar(binary->getTypeString()), 3, 4, 3, toChar("Op " + toUpper(binary->getSym())));
    }
}

void CodeGen::generateCall(Call *call)
{
    int prevToffset = m_toffset;
    emitRM("ST", 1, m_toffset, 1, "Store fp in ghost frame for", toChar(call->getName()));
    m_toffset -= 2;
    log("CodeGen::generateCall()", "TOFF: " + std::to_string(m_toffset), call->getLineNum());

    std::vector<Node *> parms = call->getParms();
    for (int i = 0; i < parms.size(); i++)
    {
        log("CodeGen::generateCall()", "Generating param " + parms[i]->stringifyWithType() + " for " + call->stringifyWithType(), call->getLineNum());
        generateNode(parms[i]);
        log("CodeGen::generateCall()", "ST 3," + std::to_string(m_toffset) + "(1) Push parameter", call->getLineNum());
        emitRM("ST", 3, m_toffset, 1, "Push parameter");
        m_toffset -= parms[i]->getMemSize();
        log("CodeGen::generateCall()", "TOFF 2: " + std::to_string(m_toffset), parms[i]->getLineNum());
    }

    emitRM("LDA", 1, prevToffset, 1, "Ghost frame becomes new active frame");
    emitRM("LDA", 3, 1, 7, "Return address in ac");
    emitRM("JMP", 7, -(emitWhereAmI() + 1 - m_funcs[call->getName()]), 7, "CALL", toChar(call->getName()));
    emitRM("LDA", 3, 0, 2, "Save the result in ac");
    m_toffset = prevToffset;
    log("CodeGen::generateCall()", "TOFF 3: " + std::to_string(m_toffset), call->getLineNum());
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
            log("CodeGen::generateConst()", "LDC 3," + std::to_string(constN->getBoolValue()) + "(6) Load Boolean constant", constN->getLineNum());
            emitRM("LDC", 3, constN->getBoolValue(), 6, "Load Boolean constant");
            break;
        case Const::Type::Char:
            emitRM("LDC", 3, (int)(constN->getCharValue()), 6, "Load char constant");
            break;
    }
}

void CodeGen::generateId(Id *id)
{
    // log("CodeGen::generateId()", "Generating Id " + id->stringifyWithType(), id->getLineNum());
    // Only load the Id if it is on the rhs of =
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
    Id *id = (Id *)unaryAsgn->getChild();
    emitRM("LD", 3, id->getMemLoc(), !id->getIsGlobal(), "load lhs variable", toChar(id->getName()));
    emitRM("LDA", 3, unaryAsgn->getTypeValue(), 3, toChar(unaryAsgn->getTypeString() + " value of"), toChar(id->getName()));
    emitRM("ST", 3, id->getMemLoc(), !id->getIsGlobal(), "Store variable", toChar(id->getName()));
    id->makeGenerated();
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
        log("CodeGen::generateEnd()", "TOFF: " + std::to_string(m_toffset), node->getLineNum());
    }
}

void CodeGen::log(const std::string loc, const std::string msg, const int lineNum)
{
    if (m_showLog)
    {
        std::cout << loc << " - " << emitWhereAmI() << ": " << msg << " (line " << lineNum << ")" << std::endl;
    }
}
