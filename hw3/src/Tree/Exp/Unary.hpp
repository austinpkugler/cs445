#pragma once

#include "Exp.hpp"

class Unary : public Exp
{
    public:
        // Enums
        enum class Type { Chsign, Sizeof, Question, Not };

        /**
         * @param lineNum Line number of occurrence.
         * @param type Type of unary operation.
         */
        Unary(const unsigned lineNum, const Unary::Type type);

        // Overridden
        std::string stringify() const override;

        // Getters
        Unary::Type getType() const { return m_type; }
        std::string getSym() const;

    private:
        const Unary::Type m_type;
};
