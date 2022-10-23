#pragma once

#include "Stmt.hpp"

class Range : public Stmt
{
    public:
        Range(const int lineNum);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Range; }
        std::string stringify() const override;
};
