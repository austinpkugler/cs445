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
        std::string stringify() const override;

        // Getters
        Binary::Type getType() const { return m_type; }
        std::string getSym() const;

    private:
        const Binary::Type m_type;
};
