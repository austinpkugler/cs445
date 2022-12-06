#pragma once

#include "Stmt.hpp"

class For : public Stmt
{
    public:
        For(const int lineNum);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::For; }
        std::string stringify() const override;

        // Getters
        unsigned getDeclCount() const;
        std::vector<Node *> getDecls() const;
};
