#pragma once

#include "Stmt.hpp"

class Break : public Stmt
{
    public:
        Break(const int lineNum);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Break; }
        std::string stringify() const override;
};
