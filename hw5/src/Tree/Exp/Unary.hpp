#pragma once

#include "Exp.hpp"

class Unary : public Exp
{
    public:
        enum class Type { Chsign, Sizeof, Question, Not };

        Unary(const int lineNum, const Unary::Type type);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Unary; }
        std::string stringify() const override;

        // Getters
        Unary::Type getType() const { return m_type; }
        std::string getSym() const;

    private:
        const Unary::Type m_type;
};
