#pragma once

#include "Stmt.hpp"

class For : public Stmt
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         */
        For(const unsigned lineNum);

        // Overridden
        std::string stringify() const;
};
