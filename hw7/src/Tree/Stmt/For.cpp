#include "For.hpp"

For::For(const int lineNum) : Stmt::Stmt(lineNum)
{
    setMemExists(true);
}

std::string For::stringify() const
{
    return "For";
}

unsigned For::getDeclCount() const
{
    return getDecls().size();
}

std::vector<Node *> For::getDecls() const
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
