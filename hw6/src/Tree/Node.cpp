#include "Node.hpp"

Node::Node(const int lineNum) : m_parent(nullptr), m_sibling(nullptr), m_siblingCount(1), m_lineNum(lineNum), m_isAnalyzed(false), m_hasMem(false), m_mem("None"), m_loc(0), m_size(1) {}

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

void Node::printGoffset()
{
    std::cout << "Offset for end of global space: " << s_goffset << std::endl;
}

Node * Node::getChild(const unsigned index) const
{
    if (index > m_children.size() - 1 || m_children.size() == 0)
    {
        return nullptr;
    }
    return m_children[index];
}

unsigned Node::getChildCount() const
{
    return m_children.size();
}

Node * Node::getRelative(const Node::Kind nodeKind) const
{
    if (m_parent == nullptr)
    {
        return nullptr;
    }
    if (m_parent->getNodeKind() == nodeKind)
    {
        return m_parent;
    }
    return m_parent->getRelative(nodeKind);
}

std::string Node::getMemStr() const
{
    std::stringstream msg;
    msg << "[mem: " << m_mem << " loc: " << m_loc << " size: " << m_size << "]";
    return msg.str();
}

void Node::printTree(const bool showTypes, const bool showMem) const
{
    static unsigned siblingCount = 0, tabCount = 0;

    printNode(showTypes);
    if (showMem && m_hasMem)
    {
        std::cout << " " << getMemStr() << " [line: " << m_lineNum << "]" << std::endl;
    }
    else
    {
        std::cout << " [line: " << m_lineNum << "]" << std::endl;
    }

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
            child->printTree(showTypes, showMem);
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
        m_sibling->printTree(showTypes, showMem);
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

void Node::addChild(Node *node)
{
    m_children.push_back(node);
    if (node == nullptr)
    {
        return;
    }
    node->m_parent = this;
    node->setSiblingParents(this);
}

void Node::addSibling(Node *node)
{
    if (this == nullptr)
    {
        return;
    }
    if (node == nullptr)
    {
        return;
    }
    node->setSiblingParents(this);
    if (m_sibling == nullptr)
    {
        m_sibling = node;
    }
    else
    {
        m_sibling->addSibling(node);
    }
    m_siblingCount++;
}

bool Node::hasRelative(const Node *node) const
{
    if (this == node)
    {
        return true;
    }
    if (m_parent == nullptr)
    {
        return false;
    }
    return m_parent->hasRelative(node);
}

bool Node::hasRelative(const Node::Kind nodeKind) const
{
    return (getRelative(nodeKind) != nullptr);
}

bool Node::parentExists() const
{
    return (m_parent != nullptr);
}

std::string Node::stringify() const
{
    return " [line: " + std::to_string(m_lineNum) + "]";
}

void Node::setSiblingParents(Node *node)
{
    if (node == nullptr)
    {
        return;
    }
    m_parent = node;
    if (m_sibling != nullptr)
    {
        m_sibling->setSiblingParents(node);
    }
}

void Node::printTabs(const unsigned tabCount) const
{
    for (int i = 0; i < tabCount; i++)
    {
        std::cout << ".   ";
    }
}
