#pragma once

#include <iostream>
#include <vector>

class Node
{
    public:
        Node();
        Node(unsigned tokenLineNum);
        Node(const unsigned tokenLineNum, const int value);
        Node(const unsigned tokenLineNum, const bool value);
        Node(const unsigned tokenLineNum, const char value);
        Node(const unsigned tokenLineNum, const std::string value);
        ~Node();

        unsigned getTokenLineNum() const { return m_tokenLineNum; }
        void printTree() const;
        void addChild(Node *node);
        void addSibling(Node *node);
        virtual void printNode() const;

    protected:
        virtual std::string stringify() const;

        const unsigned m_tokenLineNum;
        std::vector<Node *> m_children;
        Node *m_sibling;
        int m_intValue;
        bool m_boolValue;
        char m_charValue;
        std::string m_stringValue;

    private:
        void printTabs(const unsigned tabCount) const;
};
