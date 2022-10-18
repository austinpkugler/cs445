#pragma once

#include "Exp.hpp"

class Binary : public Exp
{
    public:
        // Enums
        enum class Type { Mul, Div, Mod, Add, Sub, Index, And, Or, LT, LEQ, GT, GEQ, EQ, NEQ };

        /**
         * @param lineNum Line number of occurrence.
         * @param type Type of binary operation.
         */
        Binary(const unsigned lineNum, const Binary::Type type);

        // Overridden
        std::string stringify() const;

        // Getters
        Binary::Type getType() const { return m_type; }

    private:
        const Binary::Type m_type;
};
