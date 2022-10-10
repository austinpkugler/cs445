#pragma once

#include <vector>
#include <iostream>

class Node
{
    public:
        typedef int OpKind;                                                                 // Kinds of Operators
        enum NodeKind { DeclK, StmtK, ExpK };                                               // Kinds of Statements
        enum DeclKind { VarK, FuncK, ParamK } ;                                             // Subkinds of Declarations
        enum StmtKind { NullK, IfK, WhileK, ForK, CompoundK, ReturnK, BreakK, RangeK };     // Subkinds of Statements
        enum ExpKind { OpK, ConstantK, IdK, AssignK, InitK, CallK };                        // Subkinds of Expressions
        enum VarKind { None, Local, Global, Parameter, LocalStatic };                       // What kind of scoping is used
        enum ExpType { Void, Integer, Boolean, Char, CharInt, Equal, UndefinedType };       // ExpType is used for type checking

        Node();
        Node(int tokenLineNumber);
        void addSibling(Node *node);

    private:
        std::vector<Node *> m_children;
        Node *m_sibling;
        int m_tokenLineNumber;
};
