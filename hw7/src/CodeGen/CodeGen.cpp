#include "CodeGen.hpp"

FILE *code = NULL;

CodeGen::CodeGen(const Node *root, const std::string cMinusPath, const std::string tmPath) : m_root(root), m_cMinusPath(cMinusPath), m_tmPath(tmPath) {}

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

    // emitComment("C- compiler version F22");
    // emitComment(toChar("File compiled:  " + m_cMinusPath));
    emitIO();
    emitAndTraverse(m_root);
    // emitComment("INIT");
    // emitRM("LDA", 1, 0, 0, "set first frame at end of globals");
    // emitRM("ST", 1, 0, 1, "store old fp (point to self)");
    // emitComment("INIT GLOBALS AND STATICS");
    // emitComment("END INIT GLOBALS AND STATICS");
    // emitRM("LDA", 3, 1, 7, "Return address in ac");
    // emitRM("JMP", 7, -9, 7, "Jump to main");
    // emitRO("HALT", 0, 0, 0, "DONE!");
    // emitComment("END INIT");
}

void CodeGen::generateDecl(const Decl *decl)
{
    if (decl == nullptr)
    {
        return;
    }

    switch (decl->getNodeKind())
    {
        case Node::Kind::Func:
            m_toff += decl->getMemSize();
            // emitComment("");
            // emitComment("** ** ** ** ** ** ** ** ** ** ** **");
            // emitComment(toChar("FUNCTION " + decl->getName()));
            // emitComment("TOFF set:", m_toff);
            emitRM("ST", 3, -1, 1, "Store return address");
            break;
        case Node::Kind::Parm:
        case Node::Kind::Var:
            break;
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
            // emitComment("COMPOUND");
            // emitComment("TOFF set:", m_toff);
            // emitComment("Compound Body");
            // emitComment("TOFF set:", m_toff);
            break;
        case Node::Kind::For:
        case Node::Kind::If:
        case Node::Kind::Range:
        case Node::Kind::Return:
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
    // else if (isExp(node))
    // {
    //     generateExp((Exp *)node);
    // }
    // else if (isStmt(node))
    // {
    //     generateStmt((Stmt *)node);
    // }

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
        // Func, Parm, Var, Asgn, Binary, Call, Const, Id, Unary, UnaryAsgn, Break, Compound, For, If, Range, Return, While
        case Node::Kind::Compound:
            // emitComment("END COMPOUND");
            // emitComment("Add standard closing in case there is no return statement");
            emitRM("LDC", 2, 0, 6, "Set return value to 0");
            emitRM("LD", 3, -1, 1, "Load return address");
            emitRM("LD", 1, 0, 1, "Adjust fp");
            emitRM("JMP", 7, 0, 3, "Return");
            break;
        case Node::Kind::Asgn:
            break;
        case Node::Kind::Binary:
            break;
        case Node::Kind::Call:
            break;
        case Node::Kind::Const:
            break;
        case Node::Kind::Id:
            break;
        case Node::Kind::Unary:
            break;
        case Node::Kind::UnaryAsgn:
            break;
        case Node::Kind::Func:
            // emitComment(toChar("END FUNCTION " + ((Func *)node)->getName()));
            int nextLoc = emitWhereAmI();
            emitNewLoc(0);
            emitRM("JMP", 7, 43, 7, "Jump to init [backpatch]");
            emitNewLoc(nextLoc);
            break;
    }
}

void CodeGen::emitIO() const
{
    fprintf(code, "%s", R"""(* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION input
  1:     ST  3,-1(1)	Store return address 
  2:     IN  2,2,2	Grab int input 
  3:     LD  3,-1(1)	Load return address 
  4:     LD  1,0(1)	Adjust fp 
  5:    JMP  7,0(3)	Return 
* END FUNCTION input
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION output
  6:     ST  3,-1(1)	Store return address 
  7:     LD  3,-2(1)	Load parameter 
  8:    OUT  3,3,3	Output integer 
  9:     LD  3,-1(1)	Load return address 
 10:     LD  1,0(1)	Adjust fp 
 11:    JMP  7,0(3)	Return 
* END FUNCTION output
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION inputb
 12:     ST  3,-1(1)	Store return address 
 13:    INB  2,2,2	Grab bool input 
 14:     LD  3,-1(1)	Load return address 
 15:     LD  1,0(1)	Adjust fp 
 16:    JMP  7,0(3)	Return 
* END FUNCTION inputb
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION outputb
 17:     ST  3,-1(1)	Store return address 
 18:     LD  3,-2(1)	Load parameter 
 19:   OUTB  3,3,3	Output bool 
 20:     LD  3,-1(1)	Load return address 
 21:     LD  1,0(1)	Adjust fp 
 22:    JMP  7,0(3)	Return 
* END FUNCTION outputb
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION inputc
 23:     ST  3,-1(1)	Store return address 
 24:    INC  2,2,2	Grab char input 
 25:     LD  3,-1(1)	Load return address 
 26:     LD  1,0(1)	Adjust fp 
 27:    JMP  7,0(3)	Return 
* END FUNCTION inputc
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION outputc
 28:     ST  3,-1(1)	Store return address 
 29:     LD  3,-2(1)	Load parameter 
 30:   OUTC  3,3,3	Output char 
 31:     LD  3,-1(1)	Load return address 
 32:     LD  1,0(1)	Adjust fp 
 33:    JMP  7,0(3)	Return 
* END FUNCTION outputc
* 
* ** ** ** ** ** ** ** ** ** ** ** **
* FUNCTION outnl
 34:     ST  3,-1(1)	Store return address 
 35:  OUTNL  3,3,3	Output a newline 
 36:     LD  3,-1(1)	Load return address 
 37:     LD  1,0(1)	Adjust fp 
 38:    JMP  7,0(3)	Return 
* END FUNCTION outnl
)""");
    emitNewLoc(39);
}

char * CodeGen::toChar(const std::string comment) const
{
    return const_cast<char *>((comment).c_str());
}
