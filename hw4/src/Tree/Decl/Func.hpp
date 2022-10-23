#pragma once

#include "Decl.hpp"

class Func : public Decl
{
    public:
        Func(const int lineNum, const std::string funcName, Data *data);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Func; }
        std::string stringify() const override;

        // Getters
        unsigned getParmCount() const;
        std::vector<Node *> getParms() const;
};
