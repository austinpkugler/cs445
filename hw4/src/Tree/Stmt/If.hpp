#pragma once

#include "Stmt.hpp"

class If : public Stmt
{
    public:
        /**
         * @param lineNum Line number of occurrence.
         */
        If(const unsigned lineNum);

        // Overridden
        std::string stringify() const override;
};
