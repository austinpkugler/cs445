#pragma once

#include "Stmt.hpp"

class If : public Stmt
{
    public:
        If(const int lineNum);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::If; }
        std::string stringify() const override;
};
