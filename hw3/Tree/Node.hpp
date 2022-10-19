#pragma once

#include <iostream>
#include <vector>

class Node
{
    public:
        // Enums
        enum class Kind { None, Decl, Stmt, Exp };

        /**
         * @param lineNum Line number of occurrence.
         * @param nodeKind The category of node. None if unknown.
         */
        Node(const unsigned lineNum);
        Node(const unsigned lineNum, const Node::Kind nodeKind);
        ~Node();

        // Getters
        unsigned getLineNum() const { return m_lineNum; }
        Node::Kind getNodeKind() const { return m_nodeKind; }
        std::vector<Node *> getChildren() const { return m_children; }
        Node * getSibling() const { return m_sibling; }
        Node * getParent() const { return m_parent; }

        // Setters
        void addChild(Node *node);
        void addSibling(Node *node);

        // Print
        void printTree() const;
        void printNode() const;

        // Virtual
        virtual std::string stringify() const;

    protected:
        Node *m_sibling;    // Used by decl

    private:
        void printTabs(const unsigned tabCount) const;

        const unsigned m_lineNum;
        const Node::Kind m_nodeKind;
        std::vector<Node *> m_children;
        Node *m_parent;
};
