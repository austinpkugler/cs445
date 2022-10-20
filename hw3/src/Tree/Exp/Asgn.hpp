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
        std::string stringify() const override;
        std::string stringifyWithType() const override;

        // Getters
        Asgn::Type getType() const { return m_type; }
        std::string getSym() const;

    private:
        const Asgn::Type m_type;
};
