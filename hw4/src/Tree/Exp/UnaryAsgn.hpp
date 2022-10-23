#pragma once

#include "Exp.hpp"

class UnaryAsgn : public Exp
{
    public:
        enum class Type { Inc, Dec };

        UnaryAsgn(const int lineNum, const UnaryAsgn::Type type);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::UnaryAsgn; }
        std::string stringify() const override;

        // Getters
        UnaryAsgn::Type getType() const { return m_type; }
        std::string getSym() const;

    private:
        const UnaryAsgn::Type m_type;
};
