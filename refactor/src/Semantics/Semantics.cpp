#include "Semantics.hpp"

Semantics::Semantics(SymTable *symTable) : m_symTable(symTable), m_mainExists(false) {}

void Semantics::analyze(Node *node)
{
    // Initialize the symbol table
    symTableSimpleEnterScope("Symbol table initialization");
    symTableInitialize(node);
    symTableSimpleLeaveScope();

    analyzeTree(node);

    if (!m_mainExists)
    {
        Emit::error("LINKER", "A function named 'main' with no parameters must be defined.");
    }

    // std::map<std::string, void *> syms = m_symTable->getSyms();
    // for (auto const& [name, currNode] : syms)
    // {
    //     Node *node = (Node *)currNode;
    //     if (!isDecl(node))
    //     {
    //         throw std::runtime_error("Semantics::analyze() - Illegal node found in symbol table");
    //     }

        // Decl *decl = (Decl *)node;
        // if (isFunc(decl))
        // {
        //     Func *func = (Func *)decl;
        //     if (func->getIsUsed() == false && func->getName() != "main")
        //     {
        //         Emit::warn(func->getLineNum(), "The function '" + func->getName() + "' seems not to be used.");
        //     }
        // }
    // }
}

void Semantics::analyzeTree(Node *node)
{
    if (node == nullptr || node->getIsAnalyzed())
    {
        return;
    }
    node->makeAnalyzed();

    // Analyze children
    symTableEnterScope(node);
    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        analyzeTree(children[i]);
    }
    symTableLeaveScope(node);

    // Analyze sibling
    analyzeTree(node->getSibling());
}

bool Semantics::symTableInsert(const Decl *decl, const bool global)
{
    if (!isDecl(decl))
    {
        throw std::runtime_error("Semantics::symTableInsert() - Invalid Decl");
    }

    bool inserted = false;
    if (global)
    {
        inserted = m_symTable->insertGlobal(decl->getName(), (void *)decl);
    }
    else
    {
        inserted = m_symTable->insert(decl->getName(), (void *)decl);
    }

    if (!inserted)
    {
        Decl *prevDecl = (Decl *)(symTableGet(decl->getName()));
        if (prevDecl == nullptr)
        {
            throw std::runtime_error("Semantics::symTableInsert() - Failed to insert Decl");
        }
        // std::stringstream msg;
        // msg << "Symbol '" << decl->getName() << "' is already declared at line " << prevDecl->getLineNum() << ".";
        // Emit::error(decl->getLineNum(), msg.str());
    }

    return inserted;
}

Decl * Semantics::symTableGet(const std::string name) const
{
    if (name.length() == 0)
    {
        throw std::runtime_error("Semantics::symTableGet() - Invalid name");
    }

    Decl *prevDecl = (Decl *)(m_symTable->lookup(name));
    return prevDecl;
}

void Semantics::symTableInitialize(Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    if (isDecl(node))
    {
        symTableInsert((Decl *)node);
    }

    symTableSetType(node);
    symTableEnterScope(node);

    std::vector<Node *> children = node->getChildren();
    for (int i = 0; i < children.size(); i++)
    {
        symTableInitialize(children[i]);
    }

    symTableLeaveScope(node);
    symTableInitialize(node->getSibling());
}

Data * Semantics::symTableSetType(Node *node)
{
    if (!isExp(node))
    {
        return new Data(Data::Type::Undefined, false, false);
    }

    Exp *exp = (Exp *)node;
    switch (exp->getNodeKind())
    {
        case Node::Kind::Asgn:
        {
            Asgn *asgn = (Asgn *)exp;
            if (asgn->getType() == Asgn::Type::Asgn)
            {
                exp->setData(symTableSetType(asgn->getChild())->getNextData());
            }
            else if (symTableSetType(asgn->getChild())->getType() == Data::Type::Undefined || symTableSetType(asgn->getChild(1))->getType() == Data::Type::Undefined)
            {
                exp->setData(new Data(Data::Type::Undefined, false, false));
            }
        }
        case Node::Kind::Binary:
        {
            Binary *binary = (Binary *)exp;
            if (binary->getType() == Binary::Type::Index)
            {
                exp->setData(symTableSetType(binary->getChild())->getNextData());
            }
            else if (symTableSetType(binary->getChild())->getType() == Data::Type::Undefined || symTableSetType(binary->getChild(1))->getType() == Data::Type::Undefined)
            {
                exp->setData(new Data(Data::Type::Undefined, false, false));
            }
            break;
        }
        case Node::Kind::Call:
        {
            Decl *decl = symTableGet(((Call *)node)->getName());
            if (isFunc(decl))
            {
                exp->setData(decl->getData());
            }
            break;
        }
        case Node::Kind::Id:
        {
            Decl *decl = symTableGet(((Id *)node)->getName());
            if (isVar(decl))
            {
                exp->setData(decl->getData());
            }
            break;
        }
        case Node::Kind::Unary:
        {
            Unary *unary = (Unary *)exp;
            if (symTableSetType(unary->getChild())->getType() == Data::Type::Undefined)
            {
                exp->setData(new Data(Data::Type::Undefined, false, false));
            }
            break;
        }
    }
    return exp->getData();
}

void Semantics::symTableSimpleEnterScope(const std::string name)
{
    if (name.length() == 0)
    {
        throw std::runtime_error("Semantics::symTableEnterScope() - Invalid name");
    }

    m_symTable->enter(name);
}

void Semantics::symTableSimpleLeaveScope()
{
    m_symTable->leave();
}

void Semantics::symTableEnterScope(const Node *node)
{
    switch (node->getNodeKind())
    {
        case Node::Kind::Compound:
        {
            if (node->parentExists())
            {
                Node::Kind parentKind = node->getParent()->getNodeKind();
                if (parentKind == Node::Kind::For || parentKind == Node::Kind::Func)
                {
                    return;
                }
            }
            symTableSimpleEnterScope("Compound statement");
            break;
        }
        case Node::Kind::For:
            symTableSimpleEnterScope("For loop");
            break;
        case Node::Kind::Func:
            symTableSimpleEnterScope(((Func *)node)->getName());
            break;
    }
}

void Semantics::symTableLeaveScope(const Node *node)
{
    switch (node->getNodeKind())
    {
        case Node::Kind::Compound:
        {
            if (node->parentExists())
            {
                Node::Kind parentKind = node->getParent()->getNodeKind();
                if (parentKind == Node::Kind::For || parentKind == Node::Kind::Func)
                {
                    return;
                }
            }
        }
        case Node::Kind::For:
            symTableSimpleLeaveScope();
            break;
        case Node::Kind::Func:
            symTableSimpleLeaveScope();
            break;
    }
}
