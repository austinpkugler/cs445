#pragma once

#include "Exp.hpp"

class Asgn : public Exp
{
    public:
        // Enums
        enum class Type { Asgn, AddAsgn, SubAsgn, DivAsgn, MulAsgn };

        /**
         * @param lineNum Line number of occurrence.
         * @param type Type of assignment.
         */
        Asgn(const unsigned lineNum, const Asgn::Type type);

        // Overridden
        std::string stringify() const;

        // Getters
        Asgn::Type getType() { return m_type; }

    private:
        const Asgn::Type m_type;
};