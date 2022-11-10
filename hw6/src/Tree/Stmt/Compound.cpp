#include "Compound.hpp"

Compound::Compound(const int lineNum) : Stmt::Stmt(lineNum) {}

std::string Compound::stringify() const
{
    return "Compound";
}

unsigned Compound::getDeclCount() const
{
    return getDecls().size();
}

std::vector<Node *> Compound::getDecls() const
{
    std::vector<Node *> decls;
    Node *currDecl = getChild();
    while (currDecl != nullptr)
    {
        decls.push_back(currDecl);
        currDecl = currDecl->getSibling();
    }
    return decls;
}
