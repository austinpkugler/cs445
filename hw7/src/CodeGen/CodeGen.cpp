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

    emitComment("C- compiler version F22");
    emitComment(toChar("File compiled:  " + m_cMinusPath));
    emitIO();
    emitAndTraverse(m_root);
    emitComment("INIT");
    emitComment("INIT GLOBALS AND STATICS");
    emitComment("END INIT GLOBALS AND STATICS");
    emitComment("END INIT");
}

void CodeGen::generateFuncDecl(const Func *func)
{
    if (func == nullptr)
    {
        return;
    }

    emitComment("");
    emitComment("** ** ** ** ** ** ** ** ** ** ** **");
    emitComment(toChar("FUNCTION " + func->getName()));
}

void CodeGen::generateCompoundStmt(const Compound *compound) 
{
    if (compound == nullptr)
    {
        return;
    }

    emitComment("COMPOUND");
}

void CodeGen::emitAndTraverse(const Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    switch (node->getNodeKind())
    {
        case Node::Kind::Compound:
            generateCompoundStmt((Compound *)node);
            break;
        case Node::Kind::Func:
            generateFuncDecl((Func *)node);
            break;
    }

    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        emitAndTraverse(children[i]);
    }

    switch (node->getNodeKind())
    {
        case Node::Kind::Compound:
            emitComment("END COMPOUND");
            break;
        case Node::Kind::Func:
            emitComment(toChar("END FUNCTION " + ((Func *)node)->getName()));
            break;
    }

    emitAndTraverse(node->getSibling());
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
}

char * CodeGen::toChar(const std::string comment) const
{
    return const_cast<char *>((comment).c_str());
}
