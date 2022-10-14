#include "Node.hpp"

Node::Node()
{
    m_sibling = nullptr;
}

Node::Node(unsigned tokenLineNum)
{
    m_sibling = nullptr;
    m_tokenLineNum = tokenLineNum;
}

Node::~Node()
{
    if (m_sibling != nullptr)
    {
        delete m_sibling;
    }

    for (auto &node : m_children)
    {
        delete node;
    }
}

void Node::printTree() const
{

}

void Node::addChild(Node *node)
{
    m_children.push_back(node);
}

void Node::addSibling(Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    if (m_sibling == nullptr)
    {
        m_sibling = node;
    }
    else
    {
        m_sibling->addSibling(node);
    }
}
