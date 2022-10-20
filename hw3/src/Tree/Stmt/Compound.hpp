#pragma once

#include "Stmt.hpp"

class Compound : public Stmt
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         */
        Compound(const unsigned lineNum);

        // Overridden
        std::string stringify() const;
};
