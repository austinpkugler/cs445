#include "CodeGen.hpp"

FILE *code = NULL;

CodeGen::CodeGen(const Node *root, const std::string tmPath) : m_root(root), m_tmPath(tmPath) {}

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

    emitIO();
    generateAndTraverse(m_root);
    emitRM("LDA", 1, m_goffset, 0, "set first frame at end of globals");
    emitRM("ST", 1, 0, 1, "store old fp (point to self)");

    for (int i = 0; i < m_globals.size(); i++)
    {
        Node *rhs = m_globals[i]->getChild();
        if (rhs != nullptr && isConst(rhs))
        {
            generateConst((Const *)rhs);
        }

        if (m_globals[i]->getData()->getIsArray())
        {
            emitRM("LDC", 3, m_globals[i]->getMemSize() - 1, 6, "load size of array", toChar(m_globals[i]->getName()));
            emitRM("ST", 3, 0, 0, "save size of array", toChar(m_globals[i]->getName()));
        }
        else if (!m_globals[i]->getData()->getIsStatic())
        {
            emitRM("ST", 3, m_globals[i]->getMemLoc(), 0, "Store variable", toChar(m_globals[i]->getName()));
        }
        else
        {
            emitRM("ST", 3, -2, 0, "Store variable", toChar(m_globals[i]->getName()));
        }
    }

    emitRM("LDA", 3, 1, 7, "Return address in ac");
    emitRM("JMP", 7, -(emitWhereAmI() + 1 - m_funcs["main"]), 7, "Jump to main");
    emitRO("HALT", 0, 0, 0, "DONE!");
}

void CodeGen::generateAndTraverse(const Node *node)
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

void CodeGen::generateEnd(const Node *node)
{
    switch (node->getNodeKind())
    {
        case Node::Kind::Asgn:
        {
            Node *lhs = node->getChild();
            if (lhs != nullptr && isId(lhs))
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
            break;
        }
        case Node::Kind::Func:
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
            break;
        }
    }
}

void CodeGen::generateNode(const Node *node)
{
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
            break;
        case Node::Kind::Call:
            generateCall((Call *)node);
            break;
        case Node::Kind::Const:
            if (!node->hasRelative(Node::Kind::Call) && !node->hasRelative(Node::Kind::Var) && !node->hasRelative(Node::Kind::Asgn))
            {
                generateConst((Const *)node);
            }
            break;
        case Node::Kind::Id:
            break;
        case Node::Kind::Unary:
            break;
        case Node::Kind::UnaryAsgn:
            break;
        case Node::Kind::Break:
            break;
        case Node::Kind::Compound:
            break;
        case Node::Kind::For:
            break;
        case Node::Kind::If:
            break;
        case Node::Kind::Range:
            break;
        case Node::Kind::Return:
            generateReturn((Return *)node);
            break;
        case Node::Kind::While:
            break;
    }
}

void CodeGen::generateFunc(const Func *func)
{
    m_toffset -= 2;
    emitRM("ST", 3, -1, 1, "Store return address");
    m_funcs[func->getName()] = emitWhereAmI() - 1;
}

void CodeGen::generateParm(const Parm *parm)
{
    if (parm->getData()->getIsArray())
    {
        emitRM("LDC", 3, parm->getMemSize() - 1, 6, "load size of array", toChar(parm->getName()));
        emitRM("ST", 3, -2, 1, "save size of array", toChar(parm->getName()));
    }
    m_toffset -= parm->getMemSize();
}

void CodeGen::generateVar(Var *var)
{
    if (!var->getIsGlobal())
    {
        if (var->getData()->getIsArray())
        {
            emitRM("LDC", 3, var->getMemSize() - 1, 6, "load size of array", toChar(var->getName()));
            emitRM("ST", 3, -2, 1, "save size of array", toChar(var->getName()));
        }
        Node *lhs = var->getChild();
        if (isConst(lhs))
        {
            generateConst((Const *)lhs);
        }
        else if (isUnary(lhs))
        {
            generateUnary((Unary *)lhs);
        }
        // Special case for : assignment
        if (lhs != nullptr)
        {
            emitRM("ST", 3, -2, 1, "Store variable", toChar(var->getName()));
        }
        m_toffset -= var->getMemSize();
    }
    else
    {
        m_goffset -= var->getMemSize();
        Node *rhs = var->getChild();
        if (rhs != nullptr || var->getData()->getIsArray())
        {
            m_globals.push_back(var);
        }
    }
}

void CodeGen::generateAsgn(const Asgn *asgn)
{
    if (asgn == nullptr)
    {
        return;
    }

    Node *lhs = asgn->getChild(1);
    if (isConst(lhs))
    {
        generateConst((Const *)lhs);
    }
    else if (isId(lhs))
    {
        generateId((Id *)lhs);
    }
}

void CodeGen::generateBinary(const Binary *binary)
{
    if (binary == nullptr)
    {
        return;
    }

    // Idk why this is needed
    m_toffset -= 1;

    Node *lhs = binary->getChild();
    Node *rhs = binary->getChild(1);
    if (lhs != nullptr && rhs != nullptr)
    {
        if (isConst(lhs))
        {
            generateConst((Const *)lhs);
        }
        else if (isId(lhs))
        {
            generateId((Id *)lhs);
        }

        m_toffset -= lhs->getMemSize();
        emitRM("ST", 3, m_toffset, 1, "Push left side");

        if (isConst(rhs))
        {
            generateConst((Const *)rhs);
        }
        else if (isId(rhs))
        {
            generateId((Id *)rhs);
        }

        emitRM("LD", 4, m_toffset, 1, "Pop left into ac1");
        m_toffset += lhs->getMemSize();
        emitRO(toChar(binary->getTypeString()), 3, 4, 3, toChar("Op " + toUpper(binary->getSym())));
    }
}

void CodeGen::generateCall(const Call *call)
{
    int prevToffset = m_toffset;
    emitRM("ST", 1, m_toffset, 1, "Store fp in ghost frame for", toChar(call->getName()));
    std::vector<Node *> parms = call->getParms();
    for (int i = 0; i < parms.size(); i++)
    {
        switch (parms[i]->getNodeKind())
        {
            case Node::Kind::Asgn:
                generateAsgn((Asgn *)parms[i]);
                break;
            case Node::Kind::Binary:
                generateBinary((Binary *)parms[i]);
                break;
            case Node::Kind::Const:
                generateConst((Const *)parms[i]);
                m_toffset -= parms[i]->getMemSize();
                break;
            case Node::Kind::Id:
                generateId((Id *)parms[i]);
                m_toffset -= parms[i]->getMemSize();
                break;
            case Node::Kind::Unary:
            {
                Node *rhs = parms[i]->getChild();
                if (rhs != nullptr && isId(rhs))
                {
                    Id *id = (Id *)rhs;
                    if (id->getData()->getIsArray())
                    {
                        if (id->getIsGlobal())
                        {
                            emitRM("LDA", 3, -1, 0, "Load address of base of array", toChar(id->getName()));
                        }
                        else
                        {
                            emitRM("LDA", 3, -3, 1, "Load address of base of array", toChar(id->getName()));
                        }
                        emitRM("LD", 3, 1, 3, "Load array size");
                        m_toffset -= 1;
                    }
                }
                break;
            }
        }
        emitRM("ST", 3, m_toffset - 1, 1, "Push parameter");
    }
    emitRM("LDA", 1, prevToffset, 1, "Ghost frame becomes new active frame");
    emitRM("LDA", 3, 1, 7, "Return address in ac");
    emitRM("JMP", 7, -(emitWhereAmI() + 1 - m_funcs[call->getName()]), 7, "CALL", toChar(call->getName()));
    emitRM("LDA", 3, 0, 2, "Save the result in ac");
    m_toffset = prevToffset;
}

void CodeGen::generateConst(const Const *constN)
{
    if (constN == nullptr)
    {
        return;
    }

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
    }
}

void CodeGen::generateId(const Id *id)
{
    if (id == nullptr)
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

void CodeGen::generateUnary(const Unary *unary)
{
    if (unary == nullptr)
    {
        return;
    }

    switch (unary->getType())
    {
        case Unary::Type::Chsign:
            break;
        case Unary::Type::Sizeof:
            break;
        case Unary::Type::Question:
            break;
        case Unary::Type::Not:
            Node *lhs = unary->getChild();
            if (isConst(lhs))
            {
                Const *constN = (Const *)lhs;
                generateConst(constN);
                if (constN->getType() == Const::Type::Bool)
                {
                    emitRM("LDC", 4, constN->getBoolValue(), 6, "Load 1");
                }
                emitRO("XOR", 3, 3, 4, "Op XOR to get logical not");
            }
            break;
    }
}

void CodeGen::generateReturn(const Return *returnN)
{
    emitRM("LD", 3, -1, 1, "Load return address");
    emitRM("LD", 1, 0, 1, "Adjust fp");
    emitRM("JMP", 7, 0, 3, "Return");
}

void CodeGen::emitIO()
{
    m_funcs["input"] = 1;
    m_funcs["output"] = 6;
    m_funcs["inputb"] = 12;
    m_funcs["outputb"] = 17;
    m_funcs["inputc"] = 23;
    m_funcs["outputc"] = 28;
    m_funcs["outnl"] = 34;
    fprintf(code, "%s", R"""(* ** ** ** ** ** ** ** ** ** ** ** **
* IO Library
  1:     ST  3,-1(1)	Store return address 
  2:     IN  2,2,2	Grab int input 
  3:     LD  3,-1(1)	Load return address 
  4:     LD  1,0(1)	Adjust fp 
  5:    JMP  7,0(3)	Return 
  6:     ST  3,-1(1)	Store return address 
  7:     LD  3,-2(1)	Load parameter 
  8:    OUT  3,3,3	Output integer 
  9:     LD  3,-1(1)	Load return address 
 10:     LD  1,0(1)	Adjust fp 
 11:    JMP  7,0(3)	Return 
 12:     ST  3,-1(1)	Store return address 
 13:    INB  2,2,2	Grab bool input 
 14:     LD  3,-1(1)	Load return address 
 15:     LD  1,0(1)	Adjust fp 
 16:    JMP  7,0(3)	Return 
 17:     ST  3,-1(1)	Store return address 
 18:     LD  3,-2(1)	Load parameter 
 19:   OUTB  3,3,3	Output bool 
 20:     LD  3,-1(1)	Load return address 
 21:     LD  1,0(1)	Adjust fp 
 22:    JMP  7,0(3)	Return 
 23:     ST  3,-1(1)	Store return address 
 24:    INC  2,2,2	Grab char input 
 25:     LD  3,-1(1)	Load return address 
 26:     LD  1,0(1)	Adjust fp 
 27:    JMP  7,0(3)	Return 
 28:     ST  3,-1(1)	Store return address 
 29:     LD  3,-2(1)	Load parameter 
 30:   OUTC  3,3,3	Output char 
 31:     LD  3,-1(1)	Load return address 
 32:     LD  1,0(1)	Adjust fp 
 33:    JMP  7,0(3)	Return 
 34:     ST  3,-1(1)	Store return address 
 35:  OUTNL  3,3,3	Output a newline 
 36:     LD  3,-1(1)	Load return address 
 37:     LD  1,0(1)	Adjust fp 
 38:    JMP  7,0(3)	Return 
* ** ** ** ** ** ** ** ** ** ** ** **
)""");
    emitNewLoc(39);
}

char * CodeGen::toChar(const std::string comment) const
{
    return const_cast<char *>((comment).c_str());
}

std::string CodeGen::toUpper(std::string s) const
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}
