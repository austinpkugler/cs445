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

char Node::parseFirstChar(const std::string &str) const
{
    if (str.length() == 1)
    {
        return str[0];
    }

    char parsedChar;
    if (str[0] == '\\')
    {
        if (str[1] == 'n')
        {
            return '\n';
        }
        else if (str[1] == '0')
        {
            return '\0';
        }
        else
        {
            return str[1];
        }
    }
    return str[0];
}

std::string Node::removeFirstAndLastChar(const std::string &str) const
{
    return str.substr(1, str.length() - 2);
}
