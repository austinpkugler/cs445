#include "CodeGen.hpp"

FILE *code = NULL;

CodeGen::CodeGen(const Node *root, const std::string tmPath, Semantics *analyzer) : m_root(root), m_tmPath(tmPath), m_analyzer(analyzer) {}

CodeGen::~CodeGen()
{
    fclose(code);
}

void CodeGen::printFuncs() const
{
    std::stringstream funcs;
    for (const auto &[k, v] : m_funcs)
    {
        funcs << "\"" << k << "\": " << v << ", ";
    }
    std::string mapping = funcs.str();
    mapping.erase(mapping.length() - 2);
    std::cout << "{" << mapping << "}" << std::endl;
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
    emitAndTraverse(m_root);
    emitRM("LDA", 1, m_goffset, 0, "set first frame at end of globals");
    emitRM("ST", 1, 0, 1, "store old fp (point to self)");

    for (int i = 0; i < m_globals.size(); i++)
    {
        Node *rhs = m_globals[i]->getChild();
        if (rhs != nullptr && isConst(rhs))
        {
            Const *constN = (Const *)rhs;
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
        emitRM("ST", 3, 0, 0, "Store variable", toChar(m_globals[i]->getName()));
    }

    emitRM("LDA", 3, 1, 7, "Return address in ac");
    emitRM("JMP", 7, -(emitWhereAmI() + 1 - m_funcs["main"]), 7, "Jump to main");
    emitRO("HALT", 0, 0, 0, "DONE!");
}

void CodeGen::generateDecl(Decl *decl)
{
    if (decl == nullptr)
    {
        return;
    }

    switch (decl->getNodeKind())
    {
        case Node::Kind::Func:
            m_toffset -= 2;
            emitRM("ST", 3, -1, 1, "Store return address");
            m_funcs[decl->getName()] = emitWhereAmI() - 1;
            break;
        case Node::Kind::Parm:
        case Node::Kind::Var:
        {
            Var *var = (Var *)decl;
            if (!var->getIsGlobal())
            {
                m_toffset -= decl->getMemSize();

                if (var->getChild() != nullptr)
                {
                    emitRM("ST", 3, -2, 1, "Store variable", toChar(var->getName()));
                }
            }
            else
            {
                m_goffset -= decl->getMemSize();
                m_globals.push_back(var);
            }
            break;
        }
        default:
            throw std::runtime_error("CodeGen::generateDecl - Invalid Decl");
            break;
    }
}

void CodeGen::generateExp(const Exp *exp)
{
    if (exp == nullptr)
    {
        return;
    }

    switch (exp->getNodeKind())
    {
        case Node::Kind::Asgn:
            break;
        case Node::Kind::Binary:
            break;
        case Node::Kind::Call:
        {
            int prevToffset = m_toffset;
            Call *call = (Call *)exp;
            emitRM("ST", 1, m_toffset, 1, "Store fp in ghost frame for", toChar(call->getName()));
            std::vector<Node *> parms = call->getParms();
            for (int i = 0; i < parms.size(); i++)
            {
                switch (parms[i]->getNodeKind())
                {
                    case Node::Kind::Const:
                    {
                        Const *constN = (Const *)parms[i];
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
                        m_toffset -= constN->getMemSize();
                        break;
                    }
                    case Node::Kind::Id:
                    {
                        Id *id = (Id *)parms[i];
                        Decl *decl = m_analyzer->lookupDecl(id);
                        if (decl != nullptr)
                        {
                            // This is bordering on insanity but I think if the parm is a global var you do 3,0(0)
                            emitRM("LD", 3, 0, 0, "Load variable", toChar(id->getName()));
                        }
                        else
                        {
                            // Then if it is local you do 3,-2,(1)
                            emitRM("LD", 3, -2, 1, "Load variable", toChar(id->getName()));
                        }
                        m_toffset -= id->getMemSize();
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
            break;
        }
        case Node::Kind::Const:
        {
            Const *constN = (Const *)exp;
            switch (constN->getType())
            {
                case Const::Type::Int:
                    if (!constN->hasRelative(Node::Kind::Call) && !constN->hasRelative(Node::Kind::Var))
                    {
                        emitRM("LDC", 3, constN->getIntValue(), 6, "Load integer constant");
                    }
                    break;
                case Const::Type::Bool:
                    if (!constN->hasRelative(Node::Kind::Call) && !constN->hasRelative(Node::Kind::Var))
                    {
                        emitRM("LDC", 3, constN->getBoolValue(), 6, "Load Boolean constant");
                    }
                    break;
                case Const::Type::Char:
                    if (!constN->hasRelative(Node::Kind::Call) && !constN->hasRelative(Node::Kind::Var))
                    {
                        emitRM("LDC", 3, (int)(constN->getCharValue()), 6, "Load char constant");
                    }
                    break;
            }
            break;
        }
        case Node::Kind::Id:
            break;
        case Node::Kind::Unary:
            break;
        case Node::Kind::UnaryAsgn:
            break;
        default:
            throw std::runtime_error("CodeGen::generateExp - Invalid Exp");
            break;
    }
}

void CodeGen::generateStmt(const Stmt *stmt)
{
    if (stmt == nullptr)
    {
        return;
    }

    switch (stmt->getNodeKind())
    {
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
            emitRM("LD", 3, -1, 1, "Load return address");
            emitRM("LD", 1, 0, 1, "Adjust fp");
            emitRM("JMP", 7, 0, 3, "Return");
            break;
        case Node::Kind::While:
            break;
        default:
            throw std::runtime_error("CodeGen::generateStmt - Invalid Stmt");
            break;
    }
}

void CodeGen::emitAndTraverse(const Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    if (isDecl(node))
    {
        generateDecl((Decl *)node);
    }
    else if (isExp(node))
    {
        generateExp((Exp *)node);
    }
    else if (isStmt(node))
    {
        generateStmt((Stmt *)node);
    }

    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        emitAndTraverse(children[i]);
    }

    emitEnd(node);
    emitAndTraverse(node->getSibling());
}

void CodeGen::emitEnd(const Node *node)
{
    switch (node->getNodeKind())
    {
        case Node::Kind::Asgn:
        {
            Node *lhs = node->getChild();
            if (lhs != nullptr && isId(lhs))
            {
                Id *id = (Id *)lhs;
                emitRM("ST", 3, -2, 1, "Store variable", toChar(id->getName()));
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
