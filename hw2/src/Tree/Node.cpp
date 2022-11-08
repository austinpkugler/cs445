#include "Node.hpp"

Node::Node(const unsigned tokenLineNum) : m_tokenLineNum(tokenLineNum), m_sibling(nullptr)
{

}

Node::Node(const unsigned tokenLineNum, const int value) : m_tokenLineNum(tokenLineNum), m_sibling(nullptr)
{
    m_intValue = value;
}

Node::Node(const unsigned tokenLineNum, const bool value) : m_tokenLineNum(tokenLineNum), m_sibling(nullptr)
{
    m_boolValue = value;
}

Node::Node(const unsigned tokenLineNum, const char value) : m_tokenLineNum(tokenLineNum), m_sibling(nullptr)
{
    m_charValue = value;
}

Node::Node(const unsigned tokenLineNum, const std::string value) : m_tokenLineNum(tokenLineNum), m_sibling(nullptr)
{
    m_stringValue = value;
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
    static unsigned siblingCount = 0, tabCount = 0;
    printNode();
    std::cout << " [line: " << m_tokenLineNum << "]" << std::endl;

    tabCount++;

    // Print the children
    for (int i = 0; i < m_children.size(); i++)
    {
        Node *child = m_children[i];
        if (child != nullptr)
        {
            printTabs(tabCount);
            std::cout << "Child: " + std::to_string(i) << "  ";

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

void Node::printNode() const
{
    std::cout << stringify();
}

std::string Node::stringify() const
{
    return " [line: " + std::to_string(m_tokenLineNum) + "]";
}

void Node::printTabs(const unsigned tabCount) const
{
    for (int i = 0; i < tabCount; i++)
    {
        std::cout << ".   ";
    }
}
