#pragma once

#include "Stmt.hpp"

class Compound : public Stmt
{
    public:
        Compound(const int lineNum);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Compound; }
        std::string stringify() const override;

        // Getters
        unsigned getDeclCount() const;
        std::vector<Node *> getDecls() const;
};
