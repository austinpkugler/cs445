#include "Node.hpp"

Node::Node(const unsigned lineNum) : m_lineNum(lineNum), m_nodeKind(Node::Kind::None), m_sibling(nullptr), m_parent(nullptr)
{

}

Node::Node(const unsigned lineNum, const Node::Kind nodeKind) : m_lineNum(lineNum), m_nodeKind(nodeKind), m_sibling(nullptr), m_parent(nullptr)
{

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

Node * Node::getAncestor(const Node::Kind nodeKind) const
{
    if (m_parent == nullptr)
    {
        return nullptr;
    }

    if (m_parent->getNodeKind() == nodeKind)
    {
        return m_parent;
    }

    return m_parent->getAncestor(nodeKind);
}

bool Node::ancestorHasKind(const Node::Kind nodeKind) const
{
    if (m_parent == nullptr)
    {
        return false;
    }

    if (m_parent->getNodeKind() == nodeKind)
    {
        return true;
    }

    return m_parent->ancestorHasKind(nodeKind);
}

void Node::addChild(Node *node)
{
    m_children.push_back(node);
    if (node != nullptr)
    {
        node->m_parent = this;
    }
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

void Node::printTree(const bool showTypes) const
{
    static unsigned siblingCount = 0, tabCount = 0;

    printNode(showTypes);
    std::cout << " [line: " << m_lineNum << "]" << std::endl;

    tabCount++;

    // Print the children
    for (int i = 0; i < m_children.size(); i++)
    {
        Node *child = m_children[i];
        if (child != nullptr)
        {
            printTabs(tabCount);
            std::cout << "Child: " << i << "  ";

            int tmp = siblingCount;
            siblingCount = 0;
            child->printTree(showTypes);
            siblingCount = tmp;
        }
    }

    tabCount--;
    siblingCount++;

    // Print the siblings
    if (m_sibling != nullptr)
    {
        printTabs(tabCount);
        std::cout << "Sibling: " + std::to_string(siblingCount) << "  ";
        m_sibling->printTree(showTypes);
    }

    siblingCount--;
}

void Node::printNode(const bool showTypes) const
{
    if (showTypes)
    {
        std::cout << stringifyWithType();
    }
    else
    {
        std::cout << stringify();
    }
}

std::string Node::stringify() const
{
    return " [line: " + std::to_string(m_lineNum) + "]";
}

void Node::printTabs(const unsigned tabCount) const
{
    for (int i = 0; i < tabCount; i++)
    {
        std::cout << ".   ";
    }
}
