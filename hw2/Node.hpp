#pragma once

#include <vector>
#include <iostream>

class Node
{
    public:
        typedef int OpKind;                                                                     // Kinds of Operators
        enum class NodeKind { DeclK, StmtK, ExpK };                                             // Kinds of Statements
        enum class DeclKind { VarK, FuncK, ParamK } ;                                           // Subkinds of Declarations
        enum class StmtKind { NullK, IfK, WhileK, ForK, CompoundK, ReturnK, BreakK, RangeK };   // Subkinds of Statements
        enum class ExpKind { OpK, ConstantK, IdK, AssignK, InitK, CallK };                      // Subkinds of Expressions
        enum class VarKind { None, Local, Global, Parameter, LocalStatic };                     // What kind of scoping is used
        enum class ExpType { Void, Integer, Boolean, Char, CharInt, Equal, UndefinedType };     // ExpType is used for type checking

        Node();
        Node(int tokenLineNumber);
        ~Node();
        void printTree();
        void addChild(Node *node);
        void addSibling(Node *node);

    private:
        void printNode(std::string tab);

        std::vector<Node *> m_children;
        Node *m_sibling;
        int m_tokenLineNumber;
};
