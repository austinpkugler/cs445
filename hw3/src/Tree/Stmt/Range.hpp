#pragma once

#include "Stmt.hpp"

class Range : public Stmt
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         */
        Range(const unsigned lineNum);

        // Overridden
        std::string stringify() const;
};
