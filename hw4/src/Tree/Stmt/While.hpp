#pragma once

#include "Stmt.hpp"

class While : public Stmt
{
    public:
        While(const int lineNum);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::While; }
        std::string stringify() const override;
};
