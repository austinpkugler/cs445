#include "Node.hpp"

Node::Node(const unsigned lineNum) : m_lineNum(lineNum), m_nodeKind(Node::Kind::None), m_sibling(nullptr), m_previousChild(nullptr)
{

}

Node::Node(const unsigned lineNum, const Node::Kind nodeKind) : m_lineNum(lineNum), m_nodeKind(nodeKind), m_sibling(nullptr), m_previousChild(nullptr)
{

}

Node::~Node()
{
    if (m_sibling != nullptr)
    {
        delete m_sibling;
    }

    if (m_previousChild != nullptr)
    {
        delete m_previousChild;
    }

    for (auto &node : m_children)
    {
        delete node;
    }
}

void Node::addChild(Node *node)
{
    m_children.push_back(node);
    if (node != nullptr)
    {
        m_previousChild = node;
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

void Node::printTree() const
{
    static unsigned siblingCount = 0, tabCount = 0;
    printNode();
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
            child->printTree();
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
        m_sibling->printTree();
    }

    siblingCount--;
}

void Node::printNode() const
{
    std::cout << stringify();
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
