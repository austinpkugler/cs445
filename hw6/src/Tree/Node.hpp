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

        // Static
        static int getFoffset() { return s_foffset; }
        static int getGoffset() { return s_goffset; }
        static void saveFoffset() { s_prevFoffset = s_foffset; s_foffset = 0; }
        static void resetFoffset() { s_foffset = s_prevFoffset; s_prevFoffset = 0; }
        static void decFoffset(const int dec) { s_foffset -= dec; }
        static void decGoffset(const int dec) { s_goffset -= dec; }
        static void printGoffset();

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
        void setHasMem(const bool hasMem) { m_hasMem = hasMem; }
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

        // Static
        inline static int s_prevFoffset;
        inline static int s_foffset;
        inline static int s_goffset;

        // Tree
        Node *m_parent;
        std::vector<Node *> m_children;
        unsigned m_siblingCount;

        // Analysis
        const int m_lineNum;
        bool m_isAnalyzed;

        // Memory
        bool m_hasMem;
        std::string m_memScope;
        int m_memLoc;
        int m_memSize;
};
