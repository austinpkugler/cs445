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
        std::string getMem() const { return m_mem; }
        int getSize() const { return m_size; }
        Node * getChild(const unsigned index=0) const;
        unsigned getChildCount() const;
        Node * getRelative(const Node::Kind nodeKind) const;
        std::string getMemStr() const;

        // Setters
        void makeAnalyzed() { m_isAnalyzed = true; }
        void setHasMem(const bool hasMem) { m_hasMem = hasMem; }
        void setMem(const std::string mem) { m_mem = mem; }
        void setLoc(const int loc) { m_loc = loc; }
        void setSize(const int size) { m_size = size; }

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
        void setSiblingParents(Node *node);
        void printTabs(const unsigned tabCount) const;

        // Tree
        Node *m_parent;
        std::vector<Node *> m_children;
        unsigned m_siblingCount;

        // Analysis
        const int m_lineNum;
        bool m_isAnalyzed;

        // Memory
        bool m_hasMem;
        std::string m_mem;
        int m_loc;
        int m_size;
};
