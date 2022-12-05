#include "CodeGen.hpp"

CodeGen::CodeGen(Node *root) : m_root(root) {}

CodeGen::~CodeGen()
{
    fclose(code);
}

void CodeGen::generate(const std::string cMinusPath)
{
    if (m_root == nullptr)
    {
        return;
    }

    std::filesystem::path tmPath = cMinusPath;
    tmPath = tmPath.replace_extension(".tm");
    if (!(code = fopen(tmPath.c_str(), "w")))
    {
        throw std::runtime_error("CodeGen::generate() - Invalid filepath");
    }

    emitComment("C- compiler version F22");
    emitComment(toChar("File compiled:  " + cMinusPath));
    emitIO();
    emitAndTraverse(m_root);
}

void CodeGen::generateFuncDecl(Func *func)
{
    if (func == nullptr)
    {
        return;
    }

    emitComment("");
    emitComment("** ** ** ** ** ** ** ** ** ** ** **");
    emitComment(toChar("FUNCTION " + func->getName()));
}

void CodeGen::emitAndTraverse(Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    if (isFunc(node))
    {
        generateFuncDecl((Func *)node);
    }

    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        emitAndTraverse(children[i]);
    }

    if (isFunc(node))
    {
        emitComment(toChar("END FUNCTION " + ((Func *)node)->getName()));
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

char * CodeGen::toChar(const std::string comment)
{
    return const_cast<char *>((comment).c_str());
}
