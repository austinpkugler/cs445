#pragma once

#include <iostream>
#include <vector>
#include <variant>

class Node
{
    public:
        Node();
        Node(unsigned tokenLineNum);
        ~Node();

        unsigned getTokenLineNum() const { return m_tokenLineNum; }
        void printTree() const;
        void addChild(Node *node);
        void addSibling(Node *node);

    protected:
        char parseFirstChar(const std::string &str) const;
        std::string removeFirstAndLastChar(const std::string &str) const;
        virtual void printNode() const = 0;

        unsigned m_tokenLineNum;
        std::vector<Node *> m_children;
        Node *m_sibling;
        int m_intValue;
        bool m_boolValue;
        char m_charValue;
        std::string m_stringValue;
};
