#pragma once

#include "Stmt.hpp"

class Return : public Stmt
{
    public:
        Return(const int lineNum);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Return; }
        std::string stringify() const override;
};
