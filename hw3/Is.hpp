#pragma once

#include "Tree/Tree.hpp"

// Decl nodes
bool isDecl(const Node *node);
bool isFunc(const Node *node);
bool isParm(const Node *node);
bool isVar(const Node *node);

// Exp nodes
bool isExp(const Node *node);
bool isAsgn(const Node *node);
bool isBinary(const Node *node);
bool isCall(const Node *node);
bool isConst(const Node *node);
bool isId(const Node *node);
bool isUnary(const Node *node);
bool isUnaryAsgn(const Node *node);

// Stmt nodes
bool isStmt(const Node *node);
bool isBreak(const Node *node);
bool isCompound(const Node *node);
bool isFor(const Node *node);
bool isIf(const Node *node);
bool isRange(const Node *node);
bool isReturn(const Node *node);
bool isWhile(const Node *node);
