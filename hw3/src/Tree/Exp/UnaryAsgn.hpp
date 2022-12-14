#pragma once

#include "Exp.hpp"

class UnaryAsgn : public Exp
{
    public:
        // Enums
        enum class Type { Inc, Dec };

        /**
         * @param lineNum Line number of occurrence.
         * @param type Type of unary assignment.
         */
        UnaryAsgn(const unsigned lineNum, const UnaryAsgn::Type type);

        // Overridden
        std::string stringify() const override;

        // Getters
        UnaryAsgn::Type getType() const { return m_type; }
        std::string getSym() const;

    private:
        const UnaryAsgn::Type m_type;
};
