#pragma once

#include "Exp.hpp"

class Binary : public Exp
{
    public:
        enum class Type { Mul, Div, Mod, Add, Sub, Index, And, Or, LT, LEQ, GT, GEQ, EQ, NEQ };

        Binary(const int lineNum, const Binary::Type type);

        // Overridden
        Node::Kind getNodeKind() const override { return Node::Kind::Binary; }
        std::string stringify() const override;

        // Getters
        Binary::Type getType() const { return m_type; }
        std::string getTypeString() const;
        std::string getSym() const;

    private:
        const Binary::Type m_type;
};
