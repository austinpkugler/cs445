#pragma once

#include <iostream>
#include <vector>

class Node
{
    public:
        enum class Kind { Func, Parm, Var, Asgn, Binary, Call, Const, Id, Unary, UnaryAsgn, Break, Compound, For, If, Range, Return, While };

        Node(const int lineNum);
        ~Node();

        // Getters
        int getLineNum() const { return m_lineNum; }
        bool getIsAnalyzed() { return m_isAnalyzed; }
        Node * getParent() const { return m_parent; }
        Node * getSibling() const { return m_sibling; }
        unsigned getSiblingCount() const { return m_siblingCount; }
        std::vector<Node *> getChildren() const { return m_children; }
        Node * getChild(const unsigned index=0) const;
        unsigned getChildCount() const;
        Node * getRelative(const Node::Kind nodeKind) const;

        // Setters
        void makeAnalyzed() { m_isAnalyzed = true; }
        void addChild(Node *node);
        void addSibling(Node *node);

        // Print
        void printTree(const bool showTypes=false) const;
        void printNode(const bool showTypes=false) const;

        // Helpers
        bool hasRelative(const Node *node) const;
        bool hasRelative(const Node::Kind nodeKind) const;
        bool parentExists() const;

        // Virtual
        virtual std::string stringify() const;
        virtual std::string stringifyWithType() const { return stringify(); }
        virtual Node::Kind getNodeKind() const = 0;

    protected:
        Node *m_sibling;

    private:
        void setSiblingParents(Node *node);
        void printTabs(const unsigned tabCount) const;

        const int m_lineNum;
        bool m_isAnalyzed;
        Node *m_parent;
        unsigned m_siblingCount;
        std::vector<Node *> m_children;
};
