#pragma once

#include <iostream>
#include <vector>

class Node
{
    public:
        enum NodeKind { None, Decl, Stmt, Exp };
        enum DeclKind { DeclNone, Func, Parm, Var };
        enum StmtKind { StmtNone, Break, Compound, For, If, Return, While };
        enum ExpKind { ExpNone, Asgn, Binary, Call, Const, Id, Range, Unary, UnaryAsgn };

        Node();
        Node(unsigned tokenLineNum);
        Node(const unsigned tokenLineNum, const int value);
        Node(const unsigned tokenLineNum, const bool value);
        Node(const unsigned tokenLineNum, const char value);
        Node(const unsigned tokenLineNum, const std::string value);
        ~Node();

        unsigned getTokenLineNum() const { return m_tokenLineNum; }
        std::vector<Node *> getChildren() const { return m_children; }
        Node * getSibling() const { return m_sibling; }
        void addChild(Node *node);
        void addSibling(Node *node);
        void printTree() const;

        // Virtual
        virtual int getIntValue() { return m_intValue; }
        virtual bool getBoolValue() { return m_boolValue; }
        virtual char getCharValue() { return m_charValue; }

        virtual DeclKind getDeclKind() const { return DeclKind::DeclNone; }
        virtual StmtKind getStmtKind() const { return StmtKind::StmtNone; }
        virtual ExpKind getExpKind() const { return ExpKind::ExpNone; }

        virtual std::string getStringValue() { return m_stringValue; }
        virtual NodeKind getNodeKind() const { return NodeKind::None; }
        virtual void printNode() const;

    protected:
        // Virtual
        virtual std::string stringify() const;

        const unsigned m_tokenLineNum;
        std::vector<Node *> m_children;
        Node *m_sibling;
        // Storage values
        int m_intValue;
        bool m_boolValue;
        char m_charValue;
        std::string m_stringValue;

    private:
        void printTabs(const unsigned tabCount) const;
};
