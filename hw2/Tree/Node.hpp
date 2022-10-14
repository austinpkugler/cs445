#pragma once

#include "Type.hpp"

#include <vector>
#include <iostream>

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
        virtual void printNode() const = 0;

        unsigned m_tokenLineNum;
        std::vector<Node *> m_children;
        Node *m_sibling;
};
