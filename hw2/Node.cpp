#include "Node.hpp"

Node::Node()
{

}

Node::Node(int tokenLineNumber)
{
    m_tokenLineNumber = tokenLineNumber;
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

void Node::printTree()
{

}

void Node::addChild(Node *node)
{
    m_children.push_back(node);
}

void Node::addSibling(Node *node)
{

}

void Node::printNode(std::string tab)
{
    for (int i = 0; i < tab.length(); i++)
    {
        tab += ".   ";
    }

    std::cout << tab << "Child: " << std::endl;

    for (auto child : m_children)
    {
        child->printNode(tab + ".   ");
    }

    if (m_sibling != nullptr)
    {
        m_sibling->printNode(tab);
    }
}
