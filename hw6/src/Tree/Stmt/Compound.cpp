#include "Compound.hpp"

Compound::Compound(const int lineNum) : Stmt::Stmt(lineNum)
{
    setmemExists(true);
}

std::string Compound::stringify() const
{
    return "Compound";
}

unsigned Compound::getDeclCount() const
{
    return getDecls().size();
}

std::vector<Decl *> Compound::getDecls() const
{
    std::vector<Decl *> decls;
    Node *currNode = getChild();
    while (currNode != nullptr)
    {
        Decl *currDecl = (Decl *)currNode;
        if (!currDecl->getData()->getIsStatic())
        {
            decls.push_back(currDecl);
        }
        currNode = currNode->getSibling();
    }
    return decls;
}
