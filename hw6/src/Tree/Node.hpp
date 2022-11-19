#pragma once

#include <iostream>
#include <vector>
#include <sstream>

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
        std::string getMemScope() const { return m_memScope; }
        int getMemLoc() const { return m_memLoc; }
        int getMemSize() const { return m_memSize; }
        Node * getChild(const unsigned index=0) const;
        unsigned getChildCount() const;
        Node * getRelative(const Node::Kind nodeKind) const;
        std::string getMemStr() const;

        // Setters
        void makeAnalyzed() { m_isAnalyzed = true; }
        void setmemExists(const bool memExists) { m_memExists = memExists; }
        void setMemScope(const std::string scope) { m_memScope = scope; }
        void setMemLoc(const int loc) { m_memLoc = loc; }
        void setMemSize(const int size) { m_memSize = size; }

        // Print
        void printTree(const bool showTypes=false, const bool showMem=false) const;
        void printNode(const bool showTypes=false) const;

        // Helpers
        void addChild(Node *node);
        void addSibling(Node *node);
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
        // Setters
        void setSiblingParents(Node *node);

        // Print
        void printTabs(const unsigned tabCount) const;

        // Tree
        Node *m_parent;
        std::vector<Node *> m_children;
        unsigned m_siblingCount;

        // Analysis
        const int m_lineNum;
        bool m_isAnalyzed;

        // Memory
        bool m_memExists;
        std::string m_memScope;
        int m_memLoc;
        int m_memSize;
};
